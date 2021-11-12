#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "comunication.h"

typedef struct players_sockets{
  int socket_c1;
  int socket_c2;
  int socket_c3;
  int socket_c4;
} PlayersSockets;


int prepare_socket(char * IP, int PORT);
int get_clients(int server_socket);
