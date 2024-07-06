#include "headers.h"
#include "utils.h"

void printLines(){
  for(int i = 0; i < 20; i++){
    printf("-");
  }
  printf("\n");
}

void getServerData(char** data) {
  FILE* fd = fopen("server_send.txt", "r");
  if (!fd) {
    printf(TCBRED "Unable to get the data to send\n" RESET);
    exit(1);
  }
  fseek(fd, 0, SEEK_END);
  long numChars = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  *data = (char*)malloc(sizeof(char) * (numChars + 1));
  int len = fread(*data, sizeof(char), numChars, fd);
  (*data)[len++] = '\0';
  fclose(fd);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <port>", argv[0]);
    exit(1);
  }

  char* ip = "127.0.0.1";
  char* endPtr;
  long portLong = strtol(argv[1], &endPtr, 10);
  if(*endPtr != '\0' || portLong < 0 || portLong > USHRT_MAX){
    printf(TCBRED "Entered port %s is invalid. Enter an integer in [0, 65535]\n" RESET, argv[1]);
    exit(1);
  }
  int port = (int)portLong;

  struct sockaddr_in server_addr, client_addr;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("[-]Socket Error");
    exit(1);
  }

  printf(TCBGRN "[+]UDP Server Socket created.\n" RESET);
  printf(TCBGRN "Port selected: %d\n" RESET, port);

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror(TCBRED "[-]Bind Error" RESET);
    close(sock);
    exit(1);
  }

  printLines();
  printf(TCBMAG "First, sending data from the client, receiving here (server)\n" RESET);
  getData(sock, &client_addr);
  printLines();
  printf(TCBMAG "Next, sending data from here (server), receiving on client side\n" RESET);
  sleep(1);
  sendData(sock, client_addr, getServerData);

  close(sock);
  return 0;
}