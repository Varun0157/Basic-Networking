#include <arpa/inet.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

void closeAndExit(int sock, int code) {
    if (close(sock) < 0) {
        perror("[-] close");
        exit(1);
    }
    printf(TCBGRN "[+] DISCONNECTED FROM TCP SERVER\n" RESET);
    exit(code);
}

struct sockaddr_in getSocketAddress(const char* ip, const int port) {
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    return addr;
}

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror(TCBRED "[-] socket" RESET);
        closeAndExit(sock, 1);
    }

    printf(TCBGRN "[+] TCP Server Socket created\n" RESET);
    printf(TCNGRN "port selected: %d\n" RESET, port);

    struct sockaddr_in addr = getSocketAddress(ip, port);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror(TCBRED "[-] connect" RESET);
        closeAndExit(sock, 1);
    }

    printf(TCBGRN "[+] connected to the server\n" RESET);

    char buffer[BUF_SIZE];

    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "message from TCP client\n" RESET);
    printf("send: %s", buffer);
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror(TCBRED "[-] send" RESET);
        closeAndExit(sock, 1);
    }

    memset(buffer, '\0', BUF_SIZE);
    if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
        perror("recv");
        closeAndExit(sock, 1);
    }
    printf("recv: %s\n", buffer);

    closeAndExit(sock, 0);
}
