#include<netdb.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define SERVER_PORT 8888

int main(int argc, int **argv) {

  int socket_fd = 0;
  int client_socket_fd = 0;

  int set_reuse_addr = 1;
  int max_pending_connections = 1;

  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t client_addr_len;

  if (0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "server failed to create the listenting socket\n");
    exit(1);
  }

  if (0 != setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set_reuse_addr, sizeof(set_reuse_addr))) {
    fprintf(stderr, "server failed to set SO_REUSEADDR socket option\n");
  }

  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(SERVER_PORT);
  
  if (0 > listen(socket_fd, max_pending_connections)) {
    fprintf(stderr, "server failed to listen");
    exit(1);
  } else {
    fprintf(stdout, "server listening on port %d\n", SERVER_PORT);
  }

  client_addr_len = sizeof(client);

  if (0 > (client_socket_fd = accept(socket_fd, (struct sockaddr *)&client, &client_addr_len))) {
    fprintf(stderr, "server accpet failed\n");
  } else {
    fprintf(stdout, "server accepted a client\n");
  }

  close(socket_fd);
  return 0;
}