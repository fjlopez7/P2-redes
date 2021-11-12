#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "comunication.h"
#include "conection.h"
#include <pthread.h>


char * revert(char * message){
  //Se invierte el mensaje
  
  int len = strlen(message) + 1;
  char * response = malloc(len);

  for (int i = 0; i < len-1; i++)
  {
    response[i] = message[len-2-i];
  }
  response[len-1] = '\0';
  return response;
}
int jugador_actual = 1;
PlayersSockets * players_sockets;
void * waiting_clients(void* server_socket){
  int server_socket1 = *((int *)server_socket);
  int client_socket = get_clients(server_socket1);
  
  jugador_actual++;
  
  char mess[22];
  sprintf(mess, "Bienvenido jugador %d\n", jugador_actual);
  server_send_message(client_socket, 1, mess);
  //server_send_message(players_sockets->socket_c1, 13, "Un usuario se conecto a la partida\n");
}


int main(int argc, char *argv[]){
  char * IP = "0.0.0.0";
  int PORT = 8080;
  printf("Server Projecto Redes\n");
  // Se crea el socket del servidor 
  //prepare_sockets_and_get_clients(IP,PORT);
  int server_socket = prepare_socket(IP, PORT);
  printf("Esperando jugador lider\n");
  int* s_socket = &server_socket;

  players_sockets = malloc(sizeof(players_sockets));
  players_sockets ->socket_c1 = get_clients(server_socket);
 
  char * welcome = "Bienvenido jugador 1!!\nAl ser el primer jugador, eres el líder del grupo \nAhora espera a 2 o más jugadores para empezar\n";
  server_send_message(players_sockets->socket_c1, 1, welcome);
  pthread_t thread2,thread3, thread4;
  int iret1 = pthread_create(&thread2, NULL, waiting_clients, (void*) s_socket);
  int iret2 = pthread_create(&thread3, NULL, waiting_clients, (void*) s_socket);
  int iret3 = pthread_create(&thread4, NULL, waiting_clients, (void*) s_socket);
  while (1){
    int msg_code = server_receive_id(players_sockets->socket_c1);
    
    if (msg_code == 1) //El cliente me envió un mensaje a mi (servidor)
    {
      char * client_message = server_receive_payload(players_sockets->socket_c1);
      printf("El cliente %d dice: %s\n", 1, client_message);

      // Le enviaremos el mismo mensaje invertido jeje
      char * response = revert(client_message);

      // Le enviamos la respuesta
      server_send_message(players_sockets->socket_c1, 1, response);
      break;
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
