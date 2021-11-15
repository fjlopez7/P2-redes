#include "conection.h"

//LINKS REFERENCIAS:
//https://www.man7.org/linux/man-pages/man2/socket.2.html
//https://man7.org/linux/man-pages/man7/socket.7.html
//https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
//https://man7.org/linux/man-pages/man2/setsockopt.2.html
//https://linux.die.net/man/3/htons
//https://linux.die.net/man/3/inet_aton
//https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
//https://www.man7.org/linux/man-pages/man2/bind.2.html
//https://www.man7.org/linux/man-pages/man2/accept.2.html


int prepare_socket(char * IP, int PORT){
  // Se define la estructura para almacenar info del socket del servidor al momento de su creación
  struct sockaddr_in server_addr;
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(IP, &server_addr.sin_addr);
  server_addr.sin_port = htons(PORT);
  
  int ret2 = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  int ret3 = listen(server_socket, 1);

  return server_socket;
}
int get_clients(int server_socket){
  //printf("Entre al get clients \n");
  struct sockaddr_in client_addr;
  socklen_t addr_size = sizeof(client_addr);
  return accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
}

