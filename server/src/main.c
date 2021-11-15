#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "game.h"
#include <pthread.h>

// Código obtenido
static _Atomic unsigned int cli_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static int uid = 0;

char * IP;
int PORT = 8080;
int server_socket;
PlayerInfo* players_info[4];

void queue_add(int socket_client){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < 4; ++i){
		if(players_info[i]->uid == -1){
			players_info[i] -> uid = uid++;
      players_info[i] -> socket = socket_client;
			break;
		}
	}
  
	pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void* player){
	PlayerInfo* player_actual = player;
  char mess[22];
  sprintf(mess, "Bienvenido jugador %d\n", player_actual->uid);
  server_send_message(player_actual->socket, 1, mess);
	while(1){
		
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


  return 0;
}
