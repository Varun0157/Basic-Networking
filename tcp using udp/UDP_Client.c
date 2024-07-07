#include "headers.h"
#include "networking_utils.h"
#include "utils.h"

void printLines() {
    for (int i = 0; i < 20; i++) printf("-");
    printf("\n");
}

void getClientData(char** data) { setDataToSend(data, "client_send.txt"); }

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    char* ip = "127.0.0.1";
    char* endPtr;
    long portLong = strtol(argv[1], &endPtr, 10);
    if (*endPtr != '\0' || portLong < 0 || portLong > USHRT_MAX) {
        printf(TCBRED
               "Entered port %s is invalid. Enter an integer in [0, 65535]\n" RESET,
               argv[1]);
        exit(1);
    }
    int port = (int)portLong;

    struct sockaddr_in addr;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror(TCBRED "sock" RESET);
        exit(1);
    }

    printf(TCBCYN "Port selected: %d\n" RESET, port);

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    printLines();
    printf(TCBMAG "First, sending data from here (client), receiving on server\n" RESET);
    sendData(sock, addr, getClientData);
    printLines();
    printf(TCBMAG "Next, sending data from the server, receiving here(client)\n" RESET);
    sleep(1);
    getData(sock, &addr);
    printLines();

    close(sock);
    return 0;
}