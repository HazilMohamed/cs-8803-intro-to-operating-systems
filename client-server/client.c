#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<strings.h>

#define SERVER_ADDR "localhost"
#define SERVER_PORT 8888

int main(int argc, char **argv) {

  int socket_fd = 0;
  struct sockaddr_in server_socket_addr;

  struct hostent *he = gethostbyname(SERVER_ADDR);
  unsigned long server_addr_nbo = *(unsigned long *)(he->h_addr_list[0]);

  if (0>(socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "client failer to create socket\n");
  }

  bzero(&server_socket_addr, sizeof(server_socket_addr));
  server_socket_addr.sin_family = AF_INET;
  server_socket_addr.sin_port = htons(SERVER_PORT);
  server_socket_addr.sin_addr = server_addr_nbo;

  if (0 > connect(socket_fd, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr))) {
    fprintf(stderr, "client failed to connect to %s:%d\n",SERVER_ADDR, SERVER_PORT);
    close(socket_fd);
    exit(1);
  } else {
    fprintf(stdout, "client connect to %s:%d\n",SERVER_ADDR, SERVER_PORT);
  }

  close(socket_fd);
  return 0;
}