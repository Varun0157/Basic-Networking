#include "tcp_utils.h"

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
  socklen_t addr_size;
  char buffer[BUF_SIZE];

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror(TCBRED "[-]Socket" RESET);
    close(sock);
    exit(1);
  }

  printf(TCBGRN "[+]TCP Server Socket created\n" RESET);
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
  printf(TCBGRN "Connected to the server\n" RESET);

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
    if (send(sock, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send" RESET);
      close(sock);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    if (recv(sock, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv" RESET);
      close(sock);
      exit(1);
    }
    printf(TCBCYN "Server: %s" RESET, buffer);

    int keepPlaying;
    scanf("%d", &keepPlaying);

    memset(buffer, '\0', BUF_SIZE);
    snprintf(buffer, BUF_SIZE, "%d", keepPlaying == 0 ? 0 : 1);
    printf(TCBBLU "Client: %s\n" RESET, buffer);
    if (send(sock, buffer, strlen(buffer), 0) == -1) {
      perror(TCBRED "send" RESET);
      close(sock);
      exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    if (recv(sock, buffer, sizeof(buffer), 0) == -1) {
      perror(TCBRED "recv" RESET);
      close(sock);
      exit(1);
    }
    printf(TCBYEL "Server: %s\n" RESET, buffer[0] == '0' ? "End" : "Continue");

    printLines();

    if (buffer[0] == '0') break;
  }

  close(sock);
  printf(TCBGRN "DISCONNECTED FROM TCP SERVER\n" RESET);

  return 0;
}
