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
char * IP;
int PORT = 8080;

int main (int argc, char *argv[]){
  //Se obtiene la ip y el puerto donde está escuchando el servidor (la ip y puerto de este cliente da igual)
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

  printf("Conectando al servidor de IkaRuz\n");
  // Se prepara el socket
  int server_socket = prepare_socket(IP, PORT);
  
  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1){
    int msg_code = client_receive_id(server_socket);

    // Cierres de conexión
    if (msg_code == 0) { 
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      printf("------------------\n");
      free(message);
      break;
    }
    //Igresar el nombre
    else if (msg_code == 1) { 
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
    else if (msg_code == 2) { 
      //printf("entro a msgcode==2\n");
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------\n");
      free(message);
    }

    //Repartir aldeanos:
    else if (msg_code == 3) { 
      char * message = client_receive_payload(server_socket);
      //printf("%s\n",message);
      free(message);
  
      printf("Reparte tus 9 aldeanos en los 4 roles Agricultores-Mineros-Ingenieros-Guerreros:\n");
      // los ingresa como: 3312 por ejemplo
      char * response = get_input();
      printf("------------------\n");
      client_send_message(server_socket, 3, response);
      
    }
    //Para comenzar el juego:
    else if (msg_code == 4) { 
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("Debes esperar a que todos los jugadores hayan completado su información\n");
      printf("Para comenzar el juego ingresa cualquier carácter:\n");
      char * response = get_input();
      printf("------------------\n");
      client_send_message(server_socket, 4, response);
    }
    else if (msg_code==5){
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("--------Menu--------\n");
      printf("1. Crear aldeano\n");
      printf("2. Subir nivel\n");
      printf("3. Atacar aldea\n");
      printf("4. Espiar\n");
      printf("5. Robar\n");
      printf("6. Ver mi información\n");
      printf("7. Pasar\n"); 
      printf("8. Rendirse\n"); 
      int option = getchar() - '0';
      getchar();
      printf("------------------\n");
      
      if (option==1) {  // Crear Aldeano
        client_send_message(server_socket, 5, "1");
      }
      else if (option==2) { // Subir de Nivel
        client_send_message(server_socket, 5, "2");
      }
      else if (option==3) { // Atacar Aldea
        client_send_message(server_socket, 5, "3");
      }
      else if (option==4) { // Espiar
        client_send_message(server_socket, 5, "4");
      }
      else if (option==5) { // Robar
        client_send_message(server_socket, 5, "5");
      }
      else if (option==6) { // Ver mi Info
        client_send_message(server_socket, 5, "6");   

      }
      else if (option==7) { // Pasar
        client_send_message(server_socket, 12, "");    
      }
      else if (option==8) { // Rendirse
        client_send_message(server_socket, 5, "8");     // revisar que codigo se envia al server
      }
      else {
        client_send_message(server_socket, 18, "");
      }
    } 
    else if(msg_code == 6){ // subir aldeano
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("--------Crear Aldeano--------\n");
      printf("1. Agricultor\n");
      printf("2. Minero\n");
      printf("3. Ingeniero\n");
      printf("4. Guerrero\n");
      printf("5. Volver\n");
      int option = getchar() - '0';
      getchar();
      printf("------------------\n");
      if(option == 1){
        client_send_message(server_socket, 6, "1");
      }else if(option == 2){
        client_send_message(server_socket, 6, "2");
      }else if(option == 3){
        client_send_message(server_socket, 6, "3");
      }else if(option == 4){
        client_send_message(server_socket, 6, "4");
      }else if (option == 5){
        client_send_message(server_socket, 18, "");
      }else {
        client_send_message(server_socket, 5, "1");
      }
    }
    else if(msg_code == 7){ // subir de nivel
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("--------Subir de Nivel--------\n");
      printf("1. Subir Nivel Agricultor\n");
      printf("2. Subir Nivel Minero\n");
      printf("3. Subir Nivel Ingeniero\n");
      printf("4. Subir Nivel Ataque\n");
      printf("5. Subir Nivel Defensa\n");
      printf("6. Volver\n");
      int option = getchar() - '0';
      getchar();
      printf("------------------\n");
      if (option == 1) {
        client_send_message(server_socket, 7, "1");
      }else if (option == 2) {
        client_send_message(server_socket, 7, "2");
      }else if (option == 3) {
        client_send_message(server_socket, 7, "3");
      }else if (option == 4) {
        client_send_message(server_socket, 7, "4");
      }else if (option == 5) {
        client_send_message(server_socket, 7, "5");
      }else if (option == 6) {
        client_send_message(server_socket, 18, "");
      }else {
        client_send_message(server_socket, 5, "2");
      }
    }
    else if(msg_code == 8){ // Atacar aldea
      char * message = client_receive_payload(server_socket);
      int j = message[0] - '0';
      memmove(message, message+1, strlen(message));
      printf("--------Atacar--------\n");
      printf("%s",message);
      printf("%i. Volver\n",j+1);
      free(message);
      int option = getchar() - '0';
      getchar();
      if(option<(j+1)){
        if(option==1){
          client_send_message(server_socket, 8, "0");
        }else if(option==2){
          client_send_message(server_socket, 8, "1");
        }else if(option==3){
          client_send_message(server_socket, 8, "2");
        }
      }else if(option == j+1){
        client_send_message(server_socket, 18, "");
      }else{
        client_send_message(server_socket, 5, "3");
      }
      printf("------------------------------------\n");
    }
    else if(msg_code == 9){ // Espiar a otra aldea
      char * message = client_receive_payload(server_socket);
      int j = message[0] - '0';
      memmove(message, message+1, strlen(message));
      printf("--------Espiar--------\n");
      printf("%s",message);
      printf("%i. Volver\n",j+1);
      free(message);
      int option = getchar() - '0';
      getchar();
      if(option<(j+1)){
        if(option==1){
          client_send_message(server_socket, 9, "1");
        }else if(option==2){
          client_send_message(server_socket, 9, "2");
        }else if(option==3){
          client_send_message(server_socket, 9, "3");
        }
      }else if(option == j+1){
        client_send_message(server_socket, 18, "");
      }else{
        client_send_message(server_socket, 5, "4");
      }
      printf("------------------------------------\n");
    }
    else if(msg_code == 10){ // Robar
      char * message = client_receive_payload(server_socket);
      int j = message[0] - '0';
      memmove(message, message+1, strlen(message));
      printf("--------Robar--------\n");
      printf("%s",message);
      printf("%i. Volver\n",j+1);
      free(message);
      int option = getchar() - '0';
      getchar();
      printf("--------Robar material--------\n");
      printf("1. Comida\n");
      printf("2. Oro\n");
      printf("3. Volver\n");
      
      int option1 = getchar() - '0';
      getchar();
      if(option<(j+1)){
        if(option==1){
          if(option1==1){
            client_send_message(server_socket, 10, "01");
          }else if(option1==2){
            client_send_message(server_socket, 10, "02");
          }else{
            client_send_message(server_socket, 18, "");
          }
          
        }else if(option==2){
          if(option1==1){
            client_send_message(server_socket, 10, "11");
          }else if(option1==2){
            client_send_message(server_socket, 10, "12");
          }else{
            client_send_message(server_socket, 18, "");
          }
        }else if(option==3){
          if(option1==1){
            client_send_message(server_socket, 10, "21");
          }else if(option1==2){
            client_send_message(server_socket, 10, "22");
          }else{
            client_send_message(server_socket, 18, "");
          }
        }
      }else if(option == j+1){
        client_send_message(server_socket, 18, "");
      }else{
        client_send_message(server_socket, 5, "5");
      }
      printf("------------------------------------\n");
    }
    else if(msg_code == 11){ // Solicitar info
      char * message = client_receive_payload(server_socket);
      printf("-----------Ver información-------\n");
      printf("La cantidad de cada recurso es:\n");
      printf("%s\n", message);
      free(message);
    }
    else if (msg_code == 15) { 
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      printf("------------------------------------\n");
      free(message);
      client_send_message(server_socket, 15, "");
    } 
    else if( msg_code == 16){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 17){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 18){
      char * message = client_receive_payload(server_socket);
      printf("La cantidad de aldeanos en cada rol es:\n");
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 19){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 20){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 21){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 22){
      char * message = client_receive_payload(server_socket);
      printf("El nivel de cada atributo es:\n");
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 23){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 24){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 25){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    } 
    else if( msg_code == 26){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
      printf("------------------------------------\n");
      client_send_message(server_socket, 18, "");
    } 
    else if( msg_code == 59){
      char * message = client_receive_payload(server_socket);
      printf("-----------Información clasificada del Oponente-------\n");
      free(message);
    }
    else if( msg_code == 60){
      char * message = client_receive_payload(server_socket);
      printf("%s\n", message);
      free(message);
    }
    else if( msg_code == 61){
      char * message = client_receive_payload(server_socket);
      printf("Nivel de ataque : %s\n", message);
      free(message);
    }
    else if( msg_code == 62){
      char * message = client_receive_payload(server_socket);
      printf("Nivel de defensa: %s\n", message);
      printf("------------------------------------\n");
      free(message);
    }
    else {
      char * message = client_receive_payload(server_socket);
      free(message);
      printf("Mensaje desconocido\n");
      printf("ID: %i\n", msg_code);
      printf("%s\n", message);
      printf("------------------------------------\n");
    }
    
  }
  // Se cierra el socket
  close(server_socket);

  return 0;
}