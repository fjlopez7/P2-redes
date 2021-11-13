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
    
    if (msg_code == 1) { 
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------\n");
      free(message);

      printf("Ingresa tu nombre:\n");
      char * response = get_input();
      printf("------------------\n");
      client_send_message(server_socket, 1, response);
    }
    if (msg_code == 2) { 
      char * message = client_receive_payload(server_socket);
      free(message);
      
     
      // printf("Posees 9 aldeanos y debes repartirlo en las 4 clases.\n");
      // printf("Número de agricultores:\n");
      // char * agr_s =  get_input();
      
      // int agr = agr_s[0] - '0';
      // printf("aledeano %i", agr);
      // printf("------------------ \n");
      
      // while (9< agr || 0>agr)
      // {
      //   printf("Ingresa un número válido.\n");
      //   printf("Número de agricultores:\n");
      //   agr = getchar() - '0';
        
      //   getchar();
      //   printf("------------------\n");
      // }
      
      // char buffer[2];
      // sprintf(buffer, "%d", agr); 
      // printf("here");

      client_send_message(server_socket, 2, "1");
      
    }
    if (msg_code == 3) { 
      char * message = client_receive_payload(server_socket);
      printf("mensaje: %s", message);
      free(message);
      
     
      // printf("Posees 9 aldeanos y debes repartirlo en las 4 clases.\n");
      // printf("Número de agricultores:\n");
      // char * agr_s =  get_input();
      
      // int agr = agr_s[0] - '0';
      // printf("aledeano %i", agr);
      // printf("------------------ \n");
      
      // while (9< agr || 0>agr)
      // {
      //   printf("Ingresa un número válido.\n");
      //   printf("Número de agricultores:\n");
      //   agr = getchar() - '0';
        
      //   getchar();
      //   printf("------------------\n");
      // }
      
      // char buffer[2];
      // sprintf(buffer, "%d", agr); 
      // printf("here");

      
      
    }

    
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);

  return 0;
}
