#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include "game.h"
#include <pthread.h>

int jugador_actual = 1;
PlayersSockets * players_sockets;
PlayerInfo** players_info;
int looby_states[4] = {0,0,0,0};
void * waiting_clients(void* server_socket){
  int server_socket1 = *((int *)server_socket);
  int client_socket = get_clients(server_socket1);
  
  jugador_actual++;
  
  char mess[22];
  sprintf(mess, "Bienvenido jugador %d\n", jugador_actual);
  server_send_message(client_socket, 1, mess);
  looby_states[jugador_actual-1] = 1;
  players_info[jugador_actual-1] -> socket = client_socket;
  
}


int main(int argc, char *argv[]){
  char * IP = "0.0.0.0";
  int PORT = 8080;
  printf("Server Projecto Redes\n");
  players_info = init_all_player_info();
  
  int server_socket = prepare_socket(IP, PORT);
  printf("Esperando jugador lider \n");
  int* s_socket = &server_socket;

  players_sockets = malloc(sizeof(players_sockets));
  players_sockets ->socket_c1 = get_clients(server_socket);
  players_info[0] -> socket = players_sockets ->socket_c1;
 
  char * welcome = "Bienvenido jugador 1!!\nAl ser el primer jugador, eres el líder del grupo \nAhora espera a 2 o más jugadores para empezar\n";
  server_send_message(players_sockets->socket_c1, 1, welcome);
  looby_states[0] = 1;
  printf("Jugador 1 conectado\n");
  pthread_t thread2,thread3, thread4;
  int iret1 = pthread_create(&thread2, NULL, waiting_clients, (void*) s_socket);
  int iret2 = pthread_create(&thread3, NULL, waiting_clients, (void*) s_socket);
  int iret3 = pthread_create(&thread4, NULL, waiting_clients, (void*) s_socket);
  while (1){
    int msg_code1 = server_receive_id(players_sockets->socket_c1);
    int msg_code2 = -1;
    int msg_code3 = -1;
    int msg_code4 = -1;
    if( jugador_actual == 1 ){
      printf("entre a jugador actual == 1\n");
    
    }

    if( jugador_actual == 2 ){
      printf("entre a jugador actual == 2\n");
      msg_code2 = server_receive_id(players_sockets->socket_c2);
      //printf("1) msg_code2: %i\n", msg_code2);
    }

    if( jugador_actual == 3 ){
      msg_code3 = server_receive_id(players_sockets->socket_c3);
    }

    if( jugador_actual == 4 ){
      msg_code4 = server_receive_id(players_sockets->socket_c4);
    }
    if (msg_code1 == 1){
      char * nombre = server_receive_payload(players_sockets->socket_c1);
      strcpy(players_info[0]->name,nombre);
      printf("El tipo 1 se llama: %s\n", nombre);
      server_send_message(players_sockets->socket_c1, 2, "ahhhh");
    }
    
    if (msg_code2 == 1){
      printf("el tipo2 llego al msg_code2==1\n");
      char * nombre = server_receive_payload(players_sockets->socket_c2);
      strcpy(players_info[1]->name,nombre);
      printf("El tipo 2 se llama: %s\n", nombre);
      server_send_message(players_sockets->socket_c2, 2, "");
    }
    if (msg_code3 == 1){
      char * nombre = server_receive_payload(players_sockets->socket_c3);
      strcpy(players_info[2]->name,nombre);
      server_send_message(players_sockets->socket_c3, 2, "");
    }
    if (msg_code4 == 1){
      char * nombre = server_receive_payload(players_sockets->socket_c4);
      strcpy(players_info[3]->name,nombre);
      server_send_message(players_sockets->socket_c4, 2, "");
    }
    if (msg_code1 == 2){
      char * cantidad= server_receive_payload(players_sockets->socket_c1);
      //int agr = nombre[0] - '0';
      printf("La persona 1 ingreso a susu aldeanos como: %s\n", cantidad);
      server_send_message(players_sockets->socket_c1, 3, "");
    }
    if (msg_code2 == 2){
      char * cantidad= server_receive_payload(players_sockets->socket_c1);
      printf("La persona 1 ingreso a susu aldeanos como: %s\n", cantidad);
      //int agr = agr_s[0] - '0';
      server_send_message(players_sockets->socket_c1, 3, "");
    }
    if (msg_code3 == 2){
      char * agr_s = server_receive_payload(players_sockets->socket_c3);
      
      int agr = agr_s[0] - '0';
      server_send_message(players_sockets->socket_c3, 3, "");
    }
    
  } 
  close(server_socket);
  // // Guardaremos los sockets en un arreglo e iremos alternando a quién escuchar.
  // int sockets_array[2] = {players_info->socket_c1, players_info->socket_c2};
  // int my_attention = 0;
  // while (1)
  // {
  //   // Se obtiene el paquete del cliente 1
  //   int msg_code = server_receive_id(sockets_array[my_attention]);

  //   if (msg_code == 1) //El cliente me envió un mensaje a mi (servidor)
  //   {
  //     char * client_message = server_receive_payload(sockets_array[my_attention]);
  //     printf("El cliente %d dice: %s\n", my_attention+1, client_message);

  //     // Le enviaremos el mismo mensaje invertido jeje
  //     char * response = revert(client_message);

  //     // Le enviamos la respuesta
  //     server_send_message(sockets_array[my_attention], 1, response);
  //   }
  //   else if (msg_code == 2){ //El cliente le envía un mensaje al otro cliente
  //     char * client_message = server_receive_payload(sockets_array[my_attention]);
  //     printf("Servidor traspasando desde %d a %d el mensaje: %s\n", my_attention+1, ((my_attention+1)%2)+1, client_message);

  //     // Mi atención cambia al otro socket
  //     my_attention = (my_attention + 1) % 2;

  //     server_send_message(sockets_array[my_attention], 2, client_message);
  //   }
  //   printf("------------------\n");
  // }
  close(server_socket);
  return 0;
}
