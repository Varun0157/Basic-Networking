#include <arpa/inet.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.h"

#define BUF_SIZE 1024

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
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

  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[BUF_SIZE];

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror(TCBRED "[-]Socket" RESET);
    close(sock);
    exit(1);
  }

  printf(TCBGRN "[+] TCP Server Socket created\n" RESET);
  printf(TCNGRN "Port selected: %d\n" RESET, port);

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror(TCBRED "connect" RESET);
    close(sock);
    exit(1);
  }
  printf(TCBGRN "[+] Connected to the server\n" RESET);

  memset(buffer, '\0', BUF_SIZE);
  strcpy(buffer, TCBBLU "THIS IS TCP CLIENT\n" RESET);
  printf("send: %s", buffer);
  if (send(sock, buffer, strlen(buffer), 0) == -1) {
    perror(TCBRED "send" RESET);
    close(sock);
    exit(1);
  }

  memset(buffer, '\0', BUF_SIZE);
  if (recv(sock, buffer, sizeof(buffer), 0) == -1) {
    perror("recv");
    close(sock);
    exit(1);
  }
  printf("recv: %s\n", buffer);

  if (close(sock) < 0) {
    perror("close");
    exit(1);
  }
  printf(TCBGRN "[+] DISCONNECTED FROM TCP SERVER\n" RESET);

  return 0;
}
