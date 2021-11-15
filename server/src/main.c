#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "game.h"
#include <pthread.h>
#include <string.h>


// Código obtenido
static _Atomic unsigned int cli_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static int uid = 0;

char * IP;
int PORT = 8080;
int server_socket;
static _Atomic unsigned int jugadores_listos = 0;
static _Atomic unsigned int jugadores_conectados = 0;
PlayerInfo* players_info[4];

void queue_add(int socket_client){
	pthread_mutex_lock(&clients_mutex);
  printf("socket_client: %i\n", socket_client);

	for(int i=0; i < 4; ++i){
		if(players_info[i]->uid == -1){
			players_info[i] -> uid = uid++;
      players_info[i] -> socket = socket_client;
      jugadores_conectados++;
			break;
		}
	}
  
	pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void* player){
	PlayerInfo* player_actual = player;
  char mess[22];
  char mess1[29];
  if (player_actual->uid==0){
    sprintf(mess1, "Bienvenido jugador líder %d\n", player_actual->uid);
    //printf("mi socket es: %i\n",players_info[player_actual->uid]->socket);
  }else{
    sprintf(mess1, "Bienvenido jugador %d\n", player_actual->uid);
  }
  server_send_message(player_actual->socket, 1, mess1);
  
	while(1){
    int msg_code = server_receive_id(player_actual->socket);
    if (msg_code == 1){
      char * nombre = server_receive_payload(player_actual->socket);
      //Dice en el server los jugadores conectados
      if (strlen(nombre) > 0){
        strcpy(players_info[player_actual->uid]->name,nombre);
        //printf("El jugador %i se llama: %s\n",player_actual->uid,player_actual->name);
        //Le avisa al lider los jugadores que se conectan
        if (player_actual->uid > 0){
          char mess2[90];
          sprintf(mess2, "Se ha conectado el jugador: %s\n", player_actual->name);
          server_send_message(players_info[0]->socket, 2, mess2);
        }
        server_send_message(player_actual->socket, 3, "");
      }else{
        server_send_message(player_actual->socket, 1, mess1);
      }
    }
    if (msg_code==3){
      char * aldeanos = server_receive_payload(player_actual->socket);
      if (strlen(aldeanos) < 4){
        server_send_message(player_actual->socket, 2, "Recuerda las clases son 4\n");
        server_send_message(player_actual->socket, 3, "");
        continue;
      }else{
        int agr = aldeanos[0] - '0';
        int min = aldeanos[1] - '0';
        int ing = aldeanos[2] - '0';
        int gue = aldeanos[3] - '0';
        if (agr+min+ing+gue != 9){
          server_send_message(player_actual->socket, 2, "La suma de aldeanos debe ser 9.\n");
          server_send_message(player_actual->socket, 3, "");
          continue;       
        }else{
          player_actual->agr = agr;
          player_actual->gue = gue;
          player_actual->ing = ing;
          player_actual->min = min;
          pthread_mutex_lock(&clients_mutex);
          jugadores_listos++;
          pthread_mutex_unlock(&clients_mutex);
          if (player_actual->uid==0){
            server_send_message(player_actual->socket, 4, "");
          }else{
            server_send_message(player_actual->socket, 2, "Espera al jugador lider a que empiece la partida\n");
          }
        }
      }
    }
    if (msg_code==4 && player_actual->uid==0){
      char * respuesta = server_receive_payload(player_actual->socket);
      if (jugadores_listos==jugadores_conectados && jugadores_conectados > 1){
          printf("Va a comenzar el juego\n");
          break;
      }else if(jugadores_conectados==1){
        server_send_message(player_actual->socket, 2, "Faltan jugadores por conectarse\n");
        server_send_message(player_actual->socket, 4, "");
        continue;
      }else if(jugadores_listos < jugadores_conectados){
        server_send_message(player_actual->socket, 2, "Faltan jugadores que esten listos\n");
        server_send_message(player_actual->socket, 4, "");
        continue;
      }
    }
  }
  pthread_detach(pthread_self());

	return NULL;
}




int main(int argc, char *argv[]){
  if(argc > 1){
    int i_p = 0;
    for(int i = 1; i< argc; i++){
      if(strcmp(argv[i], "-i")==0 && i+1 < argc){
        IP = argv[i+1];
        i_p = 1;
      }
      if(strcmp(argv[i], "-p")==0 && i+1 < argc){
        PORT = atoi(argv[i+1]);
      }
    }
    if(i_p==0){
      IP = "0.0.0.0";
    }
  }else{
    IP = "0.0.0.0";
  }
  printf("Server Projecto Redes mounted in: %s:%i\n",IP,PORT);
  server_socket = prepare_socket(IP, PORT);
  printf("Esperando jugadores \n");
  init_all_player_info(players_info);
  pthread_t tid;
  while(1){
		//printf("entre al while\n");
		int client_socket = get_clients(server_socket);
    printf("Llego un jugador\n");
		/* Check if max clients is reached */
		if(cli_count == 4){
      server_send_message(client_socket,0,"Max clients reached. Rejected connection\n");
			close(client_socket);
			continue;
		}
		queue_add(client_socket);
    PlayerInfo* player = players_info[cli_count];
    cli_count++;
		pthread_create(&tid, NULL, &handle_client, (void *) player);

		/* Reduce CPU usage */
		sleep(1);
	}

  free_init_all(players_info);
  close(server_socket);
  return 0;
}