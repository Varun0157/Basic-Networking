#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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
  char buffer[BUF_SIZE];
  socklen_t addr_size;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror(TCBRED "Socket" RESET);
    close(sock);
    exit(1);
  }

  printf(TCBGRN "Port selected: %d\n" RESET, port);

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  memset(buffer, '\0', BUF_SIZE);
  strcpy(buffer, TCBBLU "THIS IS UDP CLIENT" RESET);
  if (sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror(TCBRED "sendto" RESET);
    close(sock);
    exit(1);
  }
  printf("[+] sendto: %s\n", buffer);

  memset(buffer, '\0', BUF_SIZE);
  addr_size = sizeof(addr);
  if (recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_size) < 0) {
    perror(TCBRED "recvfrom" RESET);
    close(sock);
    exit(1);
  }
  printf("[+] recvfrom: %s\n", buffer);

  if (close(sock) < 0) {
    perror("close");
    exit(1);
  }
  return 0;
}