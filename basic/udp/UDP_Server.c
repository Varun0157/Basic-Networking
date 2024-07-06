#include "../utils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>", argv[0]);
        exit(1);
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

    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t addr_size;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror(TCBRED "[-]Socket Error" RESET);
        exit(1);
    }

    printf(TCBGRN "[+]UDP Server Socket created.\n" RESET);
    printf(TCNGRN "Port selected: %d\n" RESET, port);

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(TCBRED "[-]Bind Error" RESET);
        close(sock);
        exit(1);
    }

    printf(TCBGRN "Listening...\n" RESET);

    memset(buffer, '\0', BUF_SIZE);
    addr_size = sizeof(client_addr);
    if (recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size) <
        0) {
        perror(TCBRED "recvfrom" RESET);
        close(sock);
        exit(1);
    }
    printf("[+] recvfrom: %s\n", buffer);

    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "THIS IS UDP SERVER" RESET);
    if (sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr,
               sizeof(client_addr)) < 0) {
        perror(TCBRED "sendto" RESET);
        close(sock);
        exit(1);
    }
    printf("[+] sendto: %s\n", buffer);
    if (close(sock) < 0) {
        perror("close");
        exit(1);
    }

    return 0;
}