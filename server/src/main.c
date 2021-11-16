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
int game_in_progress = 0;
static _Atomic unsigned int jugadores_listos = 0;
static _Atomic unsigned int jugadores_conectados = 0;

static _Atomic unsigned int turno = 0;
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
    // Recibir nombre
    if (msg_code == 1){
      char * nombre = server_receive_payload(player_actual->socket);
      //Dice en el server los jugadores conectados
      strcpy(players_info[player_actual->uid]->name,nombre);
        //printf("El jugador %i se llama: %s\n",player_actual->uid,player_actual->name);
        //Le avisa al lider los jugadores que se conectan
      if(player_actual->uid!=0){
        char mess2[90];
        sprintf(mess2, "Se ha conectado el jugador: %s\n", player_actual->name);
        server_send_message(players_info[0]->socket, 2, mess2);
      }
      
      server_send_message(player_actual->socket, 3, "");
    }
    // Recibir aldeanos
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
    // Empezar Juego
    if (msg_code==4 && player_actual->uid==0){
      char * respuesta = server_receive_payload(player_actual->socket);
      if (jugadores_listos==jugadores_conectados && jugadores_conectados > 1){
          printf("Va empezar el juego\n");
          game_in_progress = 1;
          for(int i=0; i< 4;i++){
            if(players_info[i]-> uid != -1){
              server_send_message(players_info[i]->socket, 15, "El juego va a comenzar\n");
            }
          }
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
    if(msg_code == 15){
      char * respuesta = server_receive_payload(player_actual->socket);
      break;
    }
  }

  
  while(1){
    if (player_actual->uid==turno){
      //Se le asignan los recursos
      if(player_actual->first==1){
        add_resources(player_actual);
        server_send_message(player_actual->socket, 5, "");
        player_actual->first = 0;
      }
      
      int msg_code = server_receive_id(player_actual->socket);
      
      if (msg_code == 5){
        char * resp = server_receive_payload(player_actual->socket);
        int respuesta = resp[0] - '0';
        //quiere crear un aldeano
        if (respuesta == 1){
          //ok
          server_send_message(player_actual->socket, 6, "");
        }
        //quiere subir nivel
        if (respuesta == 2){
          //ok
          server_send_message(player_actual->socket, 7, "");
        }
        //quiere atacar 
        if (respuesta == 3){
          int j = 0;
          char name1[30], name2[30],name3[30];
          for(int i= 0;i<jugadores_listos; i++){
            if(players_info[i]->status == 1 && player_actual->uid!= players_info[i]->uid){
              if(j == 0){
                sprintf(name1, "%s", players_info[i]->name);
              }
              if(j==1){
                sprintf(name2, "%s", players_info[i]->name);
              }
              if(j==2){
                sprintf(name3, "%s", players_info[i]->name);
              }
              j++;
            }
          }
          char msg[256];
          if(j==1){
            sprintf(msg,"%i1. %s\n",j,name1);
          }else if(j==2){
            sprintf(msg,"%i1. %s\n2. %s\n",j,name1,name2);
          }else if(j==3){
            sprintf(msg,"%i1. %s\n2. %s\n3. %s\n",j,name1,name2,name3);
          }
          server_send_message(player_actual->socket, 8, msg);
        }
        //quiere espiar
        if (respuesta == 4){
          int j = 0;
          char name1[30], name2[30],name3[30];
          for(int i= 0;i<jugadores_listos; i++){
            if(players_info[i]->status == 1 && player_actual->uid!= players_info[i]->uid){
              if(j == 0){
                sprintf(name1, "%s", players_info[i]->name);
              }
              if(j==1){
                sprintf(name2, "%s", players_info[i]->name);
              }
              if(j==2){
                sprintf(name3, "%s", players_info[i]->name);
              }
              j++;
            }
          }
          char msg[256];
          if(j==1){
            sprintf(msg,"%i1. %s\n",j,name1);
          }else if(j==2){
            sprintf(msg,"%i1. %s\n2. %s\n",j,name1,name2);
          }else if(j==3){
            sprintf(msg,"%i1. %s\n2. %s\n3. %s\n",j,name1,name2,name3);
          }
          server_send_message(player_actual->socket, 9, msg);
        }
        //quiere robar
        if (respuesta == 5){
          int j = 0;
          char name4[30], name5[30],name6[30];
          for(int i= 0;i<jugadores_listos; i++){
            if(players_info[i]->status == 1 && player_actual->uid!= players_info[i]->uid){
              if(j == 0){
                sprintf(name4, "%s", players_info[i]->name);
              }
              if(j==1){
                sprintf(name5, "%s", players_info[i]->name);
              }
              if(j==2){
                sprintf(name6, "%s", players_info[i]->name);
              }
              j++;
            }
          }
          char msg[256];
          if(j==1){
            sprintf(msg,"%i1. %s\n",j,name4);
          }else if(j==2){
            sprintf(msg,"%i1. %s\n2. %s\n",j,name4,name5);
          }else if(j==3){
            sprintf(msg,"%i1. %s\n2. %s\n3. %s\n",j,name4,name5,name6);
          }

          server_send_message(player_actual->socket, 10, msg);

        }
        //quiere ver su info
        if (respuesta == 6){
          int comida = player_actual->comida;
          int oro = player_actual->oro;
          int ciencia = player_actual->ciencia;
          int agr_actual = player_actual->agr;
          int min_actual = player_actual->min; 
          int ing_actual = player_actual->ing;
          int gue_actual = player_actual->gue;
          int niv_agr_actual = player_actual->niv_agr;
          int niv_min_actual = player_actual->niv_min; 
          int niv_ing_actual = player_actual->niv_ing;
          int niv_atq_actual = player_actual->niv_atq;
          int niv_def_actual = player_actual->niv_def;
          
          char part_mess1[50];
          char part_mess2[50];
          char part_mess3[50];
          char part_mess4[50];
          char part_mess5[50];
          char part_mess6[50];
          char part_mess7[50];
          char part_mess8[50];
          char part_mess9[50];
          char part_mess10[50];
          char part_mess11[50];
          char part_mess12[50];
          
          sprintf(part_mess1, "- Comida:%i",comida);  
          sprintf(part_mess2, "- Oro:%i",oro);  
          sprintf(part_mess3, "- Ciencia:%i",ciencia);  
          sprintf(part_mess4, "- Agricultores: %i",agr_actual);  
          sprintf(part_mess5, "- Mineros: %i",min_actual);  
          sprintf(part_mess6, "- Ingenieros: %i",ing_actual);  
          sprintf(part_mess7, "- Guerreros: %i",gue_actual);  
          sprintf(part_mess8, "- Nivel agricultores: %i",niv_agr_actual);  
          sprintf(part_mess9, "- Nivel mineros: %i",niv_min_actual);  
          sprintf(part_mess10, "- Nivel ingenieros: %i",niv_ing_actual);  
          sprintf(part_mess11, "- Nivel ataque: %i",niv_atq_actual);  
          sprintf(part_mess12, "- Nivel defensa: %i",niv_def_actual);  
          
          server_send_message(player_actual->socket, 11, part_mess1);
          server_send_message(player_actual->socket, 16, part_mess2);
          server_send_message(player_actual->socket, 17, part_mess3);
          server_send_message(player_actual->socket, 18, part_mess4);
          server_send_message(player_actual->socket, 19, part_mess5);
          server_send_message(player_actual->socket, 20, part_mess6);
          server_send_message(player_actual->socket, 21, part_mess7);
          server_send_message(player_actual->socket, 22, part_mess8);
          server_send_message(player_actual->socket, 23, part_mess9);
          server_send_message(player_actual->socket, 24, part_mess10);
          server_send_message(player_actual->socket, 25, part_mess11);
          server_send_message(player_actual->socket, 26, part_mess12);
          //Cantidad de cada recurso, cantidad de cada rol, nivel de agricultores, mineros, ingenieros, ataque y defensa
        }
        //quiere pasar
        if (respuesta == 7){
          server_send_message(player_actual->socket, 12, "");
        }
        //quiere rendirse
        if (respuesta == 8){
          server_send_message(player_actual->socket, 13, "");
        }
      }
      //Va a crear un aldeano 
      else if (msg_code == 6){
        char * tipo_aldeano = server_receive_payload(player_actual->socket);
        int num_aldeano = tipo_aldeano[0] - '0';
        int created = create_villager(player_actual, num_aldeano);
        printf("creado: %i\n",created==0);
        if (created== 0){
          server_send_message(player_actual->socket, 2, "No cuenta con los recursos necesarios\n");
        }
        if(created==1){
          if(num_aldeano==1){
            server_send_message(player_actual->socket, 2, "Agricultor creado correctamente\n");
          }else if(num_aldeano==2){
            server_send_message(player_actual->socket, 2, "Minero creado correctamente\n");
          }else if(num_aldeano==3){
            server_send_message(player_actual->socket, 2, "Ingeniero creado correctamente\n");
          }else if(num_aldeano==4){
            server_send_message(player_actual->socket, 2, "Guerrero creado correctamente\n");
          }
          
        } 
        server_send_message(player_actual->socket, 5, "");
      }
      
      //Subir nivel
      else if (msg_code == 7){
        char * aldeano = server_receive_payload(player_actual->socket);
        int aldeano_elegido = aldeano[0] - '0';
        int level = level_up(player_actual, aldeano_elegido);
        if(level==-2){
          server_send_message(player_actual->socket, 2, "Maximo nivel alcanzado\n");
        }else if(level == 0 ){
          server_send_message(player_actual->socket, 2, "No cuenta con los recursos necesarios\n");
        }else{
          server_send_message(player_actual->socket, 2, "El aldeano de la clase seleccionada subio de nivel\n");
        }
        server_send_message(player_actual->socket, 5, "");
      }

      //atacar aldea
      else if (msg_code == 8){
        char* messag = server_receive_payload(player_actual->socket);
        int j_client= messag[0] - '0';
        int j = 0;
        int final_i = -1;
        char name1[30], name2[30],name3[30];
        for(int i= 0;i<jugadores_listos; i++){
          if(players_info[i]->status == 1 && player_actual->uid!= players_info[i]->uid){
            if(j_client==j){
              final_i = i;
            }
            j++;
          }
        }
        int ataque = attack(player_actual, players_info, final_i);
        int lose = 0;
        if (ataque==0){
          server_send_message(players_info[final_i]->socket, 2, "GAME OVER: Haz sido atacado y perdiste\n");
          server_send_message(player_actual->socket, 2, "Ataque fallido\n");
        }
        if (ataque==1){
          lose = 1;
          server_send_message(players_info[final_i]->socket, 2, "GAME OVER: Haz sido atacado y perdiste\n");
          server_send_message(player_actual->socket, 2, "Ataque exitoso\n");
        }
        char msgresp[90];
        sprintf(msgresp,"%s ha atacado a %s\n",player_actual->name, players_info[final_i]->name);
        char msg1[90];
        if(lose==1){
          sprintf(msg1,"%s ha perdido\n",players_info[final_i]->name);
        }
        for(int i= 0;i<jugadores_listos; i++){
            if(players_info[i]->status == 1){
              server_send_message(players_info[final_i]->socket, 2, msgresp);
              if(lose==1){
                server_send_message(players_info[final_i]->socket, 2, msg1);
              }
            }
            
          }
        server_send_message(player_actual->socket, 5, "");
      }

      //espiar
      else if (msg_code == 9){
        char * aldeano = server_receive_payload(player_actual->socket);
        int aldeano_espiado = aldeano[0] - '0';
        int espiar = spy(player_actual, aldeano_espiado);
        if (espiar==0){
          server_send_message(player_actual->socket, 2, "No tienes suficiente oro para espiar\n");
        }
        //solucionar enviar info:
        if (espiar==1){
          //(la cantidad de guerreros, nivel de ataque y defensa).
          char guerrero[100];
          char ataque[100];
          char defensa[100];
          sprintf(guerrero, "%i", players_info[aldeano_espiado]->gue);  
          sprintf(ataque, "%i",players_info[aldeano_espiado]->niv_atq);  
          sprintf(defensa, "%i",players_info[aldeano_espiado]->niv_def);  
          server_send_message(player_actual->socket, 59, "");
          server_send_message(player_actual->socket, 60, guerrero);
          server_send_message(player_actual->socket, 61, ataque);
          server_send_message(player_actual->socket, 62, defensa);
        }
        server_send_message(player_actual->socket, 5, "");
      }

      //robar
      else if (msg_code == 10){
        char * steal_msg = server_receive_payload(player_actual->socket);
        int j_client = steal_msg[0] - '0';
        int recurso = steal_msg[1] - '0';
        int j = 0;
        int target_player = -1;
        char name1[30], name2[30], name3[30];
        for (int i = 0; i<jugadores_listos; i++) {
          if(players_info[i]->status == 1 && player_actual->uid!= players_info[i]->uid){
            if(j_client==j){
              target_player = i;
            }
            j++;
          }
        }
        // Roba a otros jugadores 
        // -2 error input materia
        // -1: error id_2
        // 0: dinero insuficiente
        // 1: ok
        int can_steal = steal(player_actual, players_info, target_player, recurso);
        if (can_steal == 1) {     
          // ROBO OCURRE
          char resource_msg[7];
          if (recurso == 1) {sprintf(resource_msg,"comida");} else {sprintf(resource_msg,"oro");}
          char victim_msg[100];
          sprintf(victim_msg, "%s te ha robado %s!!!\n", player_actual->name, resource_msg);
          server_send_message(players_info[target_player]->socket, 2, victim_msg);
          char thief_msg[100];
          sprintf(thief_msg, "Le has robado %s a %s!!!\n", resource_msg, players_info[target_player]->name);
          server_send_message(player_actual->socket, 2, thief_msg);
          
          // AGREGAR MENSAJES NOTIFICANDO AL RESTO DE PLAYERS
          char notify_msg[100];
          sprintf(notify_msg, "%s le ha robado %s a %s\n!", player_actual->name, resource_msg, players_info[target_player]->name);
          for (int i = 0; i<jugadores_listos; i++) {
            if (player_actual->uid != players_info[i]->uid && target_player != i) {
              server_send_message(players_info[i]->socket, 2, notify_msg);
            }
          }

        }else if (can_steal == 0) {     
          // CIENCIA INSUFICIENTE
          server_send_message(player_actual->socket, 2, "Ciencia insuficiente!\n");
        }else if (can_steal == -1) {    
          // ERROR TARGET PLAYER
          server_send_message(player_actual->socket, 2, "Jugador objetivo invalido!\n");
        }else {                         
          // ERROR INPUT MATERIA
          server_send_message(player_actual->socket, 2, "Recurso a robar invalido!\n");
        }
        server_send_message(player_actual->socket, 5, "");
      }

      //pasar
      else if (msg_code == 12){
        char * tipo_aldeano = server_receive_payload(player_actual->socket);
        int next = -1;
        for(int i=turno+1; i<jugadores_listos; i++){
          if (players_info[i]->status == 1){
            next = i;
            break;
          }  
        }
        if(next == -1){
          for(int i=0; i<jugadores_listos; i++){
            if (players_info[i]->status == 1){
              next = i;
              break;
            }  
          }
        }
        turno = players_info[next]->uid;
        printf("turno asigando: %i\n", turno);
        player_actual->first = 1;
        server_send_message(player_actual->socket, 2, "Se acabo tu turno\n");  
      }
      //rendirse
      else if (msg_code == 13){
        
        printf("Jugador %s se rinde\n", player_actual->name);
        player_actual->status = 0;
        player_actual->ciencia = 0;
        player_actual->comida = 0;
        player_actual->oro = 0;
        player_actual->agr = 0;
        player_actual->gue = 0;
        player_actual->ing = 0;
        player_actual->min = 0;
        player_actual->uid = -1;
        player_actual -> first = 0;
        player_actual -> niv_agr = 1;
        player_actual -> niv_atq = 1;
        player_actual -> niv_def = 1;
        player_actual -> niv_ing = 1;
        player_actual -> niv_min = 1;
        //cambia el turno 
        int next = -1;
        for(int i=turno+1; i<jugadores_listos; i++){
          if (players_info[i]->status == 1){
            next = i;
            break;
          }  
        }
        if(next == -1){
          for(int i=0; i<jugadores_listos; i++){
            if (players_info[i]->status == 1){
              next = i;
              break;
            }  
          }
        }
        turno = players_info[next]->uid;
        printf("turno asigando: %i\n", turno);
        player_actual->first = 1;
        //destruye al mono
        server_send_message(player_actual->socket, 0, "Game over\n"); 
      }
      //volver
      else if( msg_code == 18){
        char * msg = server_receive_payload(player_actual->socket);
        server_send_message(player_actual->socket, 5, "");
      }
      else {
        
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
    if (game_in_progress) {
      printf("Se intentó conectar un jugador pero fue kickeado\n");
      server_send_message(client_socket,0,"Game has already started.\nExiting...\n");
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