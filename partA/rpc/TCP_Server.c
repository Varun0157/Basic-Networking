#include <arpa/inet.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.h"

#define BUF_SIZE 1024

void printLines() {
  for (int i = 0; i < 10; i++) {
    printf("-");
  }
  printf("\n");
}

char* getMove(int move) {
  if (move == 0)
    return "rock";
  else if (move == 1)
    return "paper";
  else
    return "scissors";
}

char* getResult(int res, int client) {
  if (res == 0) return "\033[1;33mDRAW!\033[0m";

  if (client == 1) {
    if (res == 1) {
      return "\033[1;32mVICTORY!\033[0m";
    } else {
      return "\033[1;31mLOSS!\033[0m";
    }
  } else {
    if (res == 1) {
      return "\033[1;31mLOSS!\033[0m";
    } else {
      return "\033[1;32mVICTORY!\033[0m";
    }
  }
}

char* getServerResult(int res) {
  if (res == 0)
    return "DRAW";
  else if (res == 1)
    return "CLIENT 1 WINS";
  else
    return "CLIENT 2 WINS";
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s <port1> <port2>", argv[0]);
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
  int port1 = (int)portLong;

  char* endPtr2;
  long portLong2 = strtol(argv[2], &endPtr2, 10);
  if (*endPtr2 != '\0' || portLong2 < 0 || portLong2 > USHRT_MAX) {
    printf(TCBRED "Entered port %s is invalid. Enter an integer in [0, 65535]\n" RESET,
           argv[2]);
    exit(1);
  }
  int port2 = (int)portLong2;

  int server_sock1, server_sock2;
  struct sockaddr_in server_addr1, server_addr2;

  server_sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock1 < 0) {
    perror(TCBRED "[-]Socket Error" RESET);
    exit(1);
  }

  server_sock2 = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock2 < 0) {
    perror(TCBRED "[-]Socket Error" RESET);
    close(server_sock1);
    exit(1);
  }

  printf(TCBGRN "[+]TCP Server Socket created.\n" RESET);
  printf(TCNGRN "Port selected: %d and %d\n" RESET, port1, port2);

  memset(&server_addr1, '\0', sizeof(server_addr1));
  server_addr1.sin_family = AF_INET;
  server_addr1.sin_port = htons(port1);
  server_addr1.sin_addr.s_addr = inet_addr(ip);

  memset(&server_addr2, '\0', sizeof(server_addr2));
  server_addr2.sin_family = AF_INET;
  server_addr2.sin_port = htons(port2);
  server_addr2.sin_addr.s_addr = inet_addr(ip);

  if (bind(server_sock1, (struct sockaddr*)&server_addr1, sizeof(server_addr1)) < 0) {
    char err[BUF_SIZE];
    snprintf(err, BUF_SIZE, TCBRED "[-]Bind Error on port %d" RESET, port1);
    perror(err);
    close(server_sock1);
    close(server_sock2);
    exit(1);
  }

  if (bind(server_sock2, (struct sockaddr*)&server_addr2, sizeof(server_addr2)) < 0) {
    char err[BUF_SIZE];
    snprintf(err, BUF_SIZE, TCBRED "[-]Bind Error on port %d" RESET, port2);
    perror(err);
    close(server_sock1);
    close(server_sock2);
    exit(1);
  }

  printf(TCBGRN "[+]Bound to the port numbers %d and %d\n" RESET, port1, port2);

  if (listen(server_sock1, 5) == -1) {
    perror(TCBRED "listen error in first socket" RESET);
    close(server_sock1);
    close(server_sock2);
    exit(1);
  }

  if (listen(server_sock2, 5) == -1) {
    perror(TCBRED "listen error in second socket" RESET);
    close(server_sock1);
    close(server_sock2);
    exit(1);
  }
  printf(TCBGRN "Listening...\n" RESET);

  socklen_t addr_size;
  char buffer[BUF_SIZE];

  addr_size = sizeof(server_addr1);
  int client_sock1;
  if ((client_sock1 =
           accept(server_sock1, (struct sockaddr*)&server_addr1, &addr_size)) == -1) {
    perror(TCBRED "accept" RESET);
    close(server_sock1);
    close(server_sock2);
    exit(1);
  }
  printf(TCBGRN "[+]Client 1 Connected\n" RESET);

  addr_size = sizeof(server_addr2);
  int client_sock2;
  if ((client_sock2 =
           accept(server_sock2, (struct sockaddr*)&server_addr2, &addr_size)) == -1) {
    perror(TCBRED "accept" RESET);
    close(server_sock1);
    close(server_sock2);
    close(client_sock1);
    exit(1);
  }
  printf(TCBGRN "[+]Client 2 Connected\n" RESET);

  while (1) {
    printLines();

    memset(buffer, '\0', BUF_SIZE);
    if (recv(client_sock1, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv in client socket 1" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    int move1 = atoi(buffer);
    printf(TCBBLU "Client 1 played: %s\n" RESET, getMove(move1));

    memset(buffer, '\0', BUF_SIZE);
    if (recv(client_sock2, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv in client socket 2" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    int move2 = atoi(buffer);
    printf(TCBBLU "Client 2 played: %s\n", getMove(move2));

    int res = (move1 - move2 + 3) % 3;
    printf("%s\n", getServerResult(res));

    memset(buffer, '\0', BUF_SIZE);
    snprintf(buffer, BUF_SIZE,
             "%s\nPlayer 2 played %s\nEnter 0 to stop, or 1 to continue: ",
             getResult(res, 1), getMove(move2));

    if (send(client_sock1, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send error to client sock 1" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    snprintf(buffer, BUF_SIZE,
             "%s\nPlayer 1 played %s\nEnter 0 to stop, or 1 to continue: ",
             getResult(res, 2), getMove(move1));

    if (send(client_sock2, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send error to client sock 2" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    if (recv(client_sock1, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv in client socket 1" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    int cont1 = atoi(buffer);

    memset(buffer, '\0', BUF_SIZE);
    if (recv(client_sock2, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv in client socket 2" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    int cont2 = atoi(buffer);

    memset(buffer, '\0', BUF_SIZE);
    int cont = cont1 && cont2;
    if (cont) {
      snprintf(buffer, BUF_SIZE, "%d", 1);
    } else {
      snprintf(buffer, BUF_SIZE, "%d", 0);
    }

    if (send(client_sock1, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send error to client sock 1" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    if (send(client_sock2, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send error to client sock 2" RESET);
      close(server_sock1);
      close(server_sock2);
      close(client_sock1);
      close(client_sock2);
      exit(1);
    }

    printLines();
    if (!cont) break;
  }

  close(client_sock1);
  close(client_sock2);
  printf(TCBGRN "[+]Clients Disconnecetd\n\n" RESET);

  close(server_sock1);
  close(server_sock2);

  return 0;
}
