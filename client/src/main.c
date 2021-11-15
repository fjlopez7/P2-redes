#include <unistd.h>
#include <stdio.h>
#include "conection.h"
#include "comunication.h"

char * get_input(){
  char * response = malloc(20);
  int pos=0;
  while (1){
    char c = getchar();
    if (c == '\n') break;
    response[pos] = c;
    pos++;
  }
  response[pos] = '\0';
  return response;
}


int main (int argc, char *argv[]){
  //Se obtiene la ip y el puerto donde está escuchando el servidor (la ip y puerto de este cliente da igual)
  char * IP = "0.0.0.0";
  int PORT = 8080;

  printf("Conectando al servidor de IkaRuz\n");
  // Se prepara el socket
  int server_socket = prepare_socket(IP, PORT);
  
  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1){
    int msg_code = client_receive_id(server_socket);

    if (msg_code == 0) { 
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------\n");
      free(message);
      break;
    }
    //Igresar el nombre
    if (msg_code == 1) { 
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------\n");
      free(message);

      printf("Ingresa tu nombre:\n");
      char * response = get_input();
      client_send_message(server_socket, 1, response);
      printf("------------------\n");
    }

    //Se le mandan los jugadores conectados al lider
    if (msg_code == 2) { 
      //printf("entro a msgcode==2\n");
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------\n");
      free(message);
    }

    //Repartir aldeanos:
    if (msg_code == 3) { 
      char * message = client_receive_payload(server_socket);
      //printf("%s\n",message);
      free(message);
  
      printf("Reparte tus 9 aldeanos en los 4 roles Agicultores-Mineros-Ingenieros-Guerreros:\n");
      // los ingresa como: 3312 por ejemplo
      char * response = get_input();
      printf("------------------\n");
      client_send_message(server_socket, 3, response);
      
    }
    //Para comenzar el juego:
    if (msg_code == 4) { 
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("Debes esperar a que todos los jugadores hayan completado su información\n");
      printf("Para comenzar el juego ingresa cualquier carácter:\n");
      char * response = get_input();
      printf("------------------\n");
      client_send_message(server_socket, 4, response);
    }

    
  }

  // Se cierra el socket
  close(server_socket);

  return 0;
}