#include<netdb.h>
#include<netinet/in.h>
#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define SERVER_ADDR "localhost"
#define SERVER_PORT 8888
#define BUFF_SIZE 1024
#define MSG "hEllO wOrld"

int main(int argc, char **argv) {
  
  int socket_fd = 0;
  struct sockaddr_in server_socket_addr;
  char buffer[BUFF_SIZE];

  struct hostent *he = gethostbyname(SERVER_ADDR);
  unsigned long server_addr_nbo = *(unsigned long *)(he->h_addr_list[0]);

  if(0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "client failed to create socket\n");
    exit(1);
  }

  bzero(&server_socket_addr, sizeof(server_socket_addr));
  server_socket_addr.sin_port = htons(SERVER_PORT);
  server_socket_addr.sin_family = AF_INET;
  server_socket_addr.sin_addr.s_addr = server_addr_nbo;

  if (0 > connect(socket_fd, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr))) {
    fprintf(stderr, "client failed to connect to %s:%d!\n", SERVER_ADDR, SERVER_PORT);
    exit(1);
  } else {
    fprintf(stdout, "client  connected to %s:%d!\n", SERVER_ADDR, SERVER_PORT);
  }

  if (0 > send(socket_fd, MSG, strlen(MSG),0)) {
    fprintf(stderr, "client failed to send message\n");
    close(socket_fd);
    exit(1);
  }

  bzero(buffer, BUFF_SIZE);
  if (0 > read(socket_fd, MSG, BUFF_SIZE)) {
    fprintf(stderr, "client failed to read response from server\n");
    close(socket_fd);
    exit(1);
  } else {
    fprintf(stdout, "echo from server: %s\n", buffer);
  }

  close(socket_fd);
  return 0;
}
