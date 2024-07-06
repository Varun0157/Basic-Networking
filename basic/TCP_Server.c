#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.h"

#define BUF_SIZE 1024

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <port>", argv[0]);
    exit(1);
  }

  char* ip = "127.0.0.1";
  char* endPtr;
  long portLong = strtol(argv[1], &endPtr, 10);
  if (*endPtr != '\0' || portLong < 0 || portLong > USHRT_MAX) {
    printf(TCBRED "Entered port %s is invalid. Enter an integer in [0, 65535]\n" RESET,
           argv[1]);
    exit(1);
  }
  int port = (int)portLong;

  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;

  socklen_t addr_size;
  char buffer[BUF_SIZE];

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    perror(TCBRED "[-]Socket Error" RESET);
    exit(1);
  }

  printf(TCBGRN "[+]TCP Server Socket created.\n" RESET);
  printf(TCNGRN "Port selected: %d\n" RESET, port);

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror(TCBRED "[-]Bind Error" RESET);
    close(server_sock);
    exit(1);
  }

  printf(TCBGRN "[+]Bound to the port number %d\n" RESET, port);

  if (listen(server_sock, 5) == -1) {
    perror(TCBRED "listen" RESET);
    close(server_sock);
    exit(1);
  }
  printf(TCBGRN "Listening...\n" RESET);

  addr_size = sizeof(client_addr);
  if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size)) ==
      -1) {
    perror(TCBRED "accept" RESET);
    close(client_sock);
    close(server_sock);
    exit(1);
  }
  printf(TCBGRN "[+]Client Connected\n" RESET);

  memset(buffer, '\0', BUF_SIZE);
  if (recv(client_sock, buffer, sizeof(buffer), 0) == -1) {
    perror("recv");
    close(client_sock);
    close(server_sock);
    exit(1);
  }
  printf("recv: %s", buffer);

  memset(buffer, '\0', BUF_SIZE);
  strcpy(buffer, TCBBLU "THIS IS TCP SERVER" RESET);
  printf("send: %s\n", buffer);
  if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
    perror("send");
    close(client_sock);
    close(server_sock);
    exit(1);
  }

  if (close(client_sock) < 0) {
    perror("close");
    if (close(server_sock) < 0) {
      perror("close");
      exit(1);
    }
    exit(1);
  }
  printf(TCBGRN "[+]Client Disconnecetd\n\n" RESET);

  if (close(server_sock) < 0) {
    perror("close");
    exit(1);
  }

  return 0;
}
