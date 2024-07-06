#include <arpa/inet.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "headers.h"

#define BUF_SIZE 1024

void printLines() {
  for (int i = 0; i < 10; i++) {
    printf("-");
  }
  printf("\n");
}

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
  printf(TCBGRN "[+]UDP Server Socket created\n" RESET);
  printf(TCNGRN "Port selected: %d\n" RESET, port);

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  while (1) {
    printLines();

    int input = -1;
    while (1) {
      printf(TCBMAG "Enter 0 for rock, 1 for paper, 2 for scissors: " RESET);
      scanf("%d", &input);
      if (input != 0 && input != 1 && input != 2) {
        printf(TCBRED "Invalid, try again\n" RESET);
      } else {
        break;
      }
    }

    memset(buffer, '\0', BUF_SIZE);
    snprintf(buffer, BUF_SIZE, "%d", input);
    printf(TCBBLU "Client: %s\n" RESET, buffer);
    if (sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      perror("sendto");
      close(sock);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    addr_size = sizeof(addr);
    if (recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_size) < 0) {
      perror(TCBRED "recvfrom" RESET);
      close(sock);
      exit(1);
    }
    printf(TCBYEL "Server: %s" RESET, buffer);

    int keepPlaying;
    scanf("%d", &keepPlaying);

    memset(buffer, '\0', BUF_SIZE);
    snprintf(buffer, BUF_SIZE, "%d", keepPlaying == 0 ? 0 : 1);
    printf(TCBBLU "Client: %s\n" RESET, buffer);
    if (sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      perror("sendto");
      close(sock);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    addr_size = sizeof(addr);
    if (recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_size) < 0) {
      perror("recvfrom");
      close(sock);
      exit(1);
    }
    printf("Server: %s\n", buffer[0] == '0' ? "End" : "Continue");
    printLines();

    if (buffer[0] == '0') break;
  }

  close(sock);

  return 0;
}