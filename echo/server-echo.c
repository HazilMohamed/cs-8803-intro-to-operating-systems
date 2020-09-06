#include<netdb.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<netinet/in.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>

#define SERVER_PORT 8888
#define BUF_SIZE 1024

void capitalizeBuffer(char *buffer) {
  do {
    *buffer = toupper((unsigned char) *buffer);
  } while (*buffer++);
}

int main(int argc, int **argv) {

  int socket_fd = 0;
  int client_socket_fd = 0;

  char buffer[BUF_SIZE];
  int num_bytes = 0;

  int set_reuse_addr = 1;
  int max_pending_conn = 1;

  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *client_host_info;
  char *client_host_ip;
  socklen_t client_addr_len;

  if (0 > (socket(AF_INET, SOCK_STREAM,0))) {
    fprintf(stderr, "failed to create listening socket\n");
    exit(1);
  }

  if (0 != setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set_reuse_addr, sizeof(set_reuse_addr))) {
    fprintf(stderr, "server failed to set SO_REUSEADDR socket option\n");
  }

  bzero(&server, sizeof(server));
  server.sin_port = htons(SERVER_PORT);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (0 > bind(socket_fd, (struct sockaddr *)&server, sizeof(server))) {
    fprintf(stderr, "server failed to bind\n");
    exit(1);
  } else {
    fprintf(stdout, "server listening for a connection on port %d\n", SERVER_PORT);
  }

  client_addr_len = sizeof(client);

  if (0 > (client_socket_fd = accept(socket_fd, (struct sockaddr *)&client, &client_addr_len))) {
    fprintf(stderr, "server accept failed\n");
  } else {
    fprintf(stdout, "server accepted a client\n");
  }

  client_host_info = gethostbyaddr((const char *)&client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), AF_INET);
  if (client_host_info == NULL) {
    fprintf(stderr, "server could not determine client host address\n");
  }
  client_host_ip = inet_ntoa(client.sin_addr);
  if (client_host_ip == NULL) {
    fprintf(stderr, "server could not determine client host\n");
  } else {
    fprintf(stdout, "server established connection with %s (%s)\n", client_host_info->h_name, client_host_ip);
  }

  bzero(buffer, BUF_SIZE);
  num_bytes = read(client_socket_fd, buffer, BUF_SIZE);
  if (num_bytes == 0) {
    fprintf(stderr, "server could not read from socket\n");
  } else {
    fprintf(stdout, "server received %d bytes: %s\n", num_bytes, buffer);
  }

  capitalizeBuffer(buffer);

  if (0 > write(client_socket_fd, buffer, sizeof(buffer))) {
    fprintf(stderr, "server could not write back\n");
  } else {
    fprintf(stdout, "server sending message back to client\n");
  }

  close(socket_fd);
  return 0;
}