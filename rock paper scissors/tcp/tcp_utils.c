#include "tcp_utils.h"

void closeClientSocket(int sock) {
    if (closeSocket(sock) == 0) printf(TCBGRN "[+] socket closed\n" RESET);
}

int createSocketTCP() {
    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror(TCBRED "[-] socket " RESET);
        exit(1);
    }

    printf(TCBGRN "[+] TCP Server Socket created\n" RESET);
    return sock;
}

int sendMessage(int sock, char buffer[BUF_SIZE]) {
    if (send(sock, buffer, BUF_SIZE, 0) < 0) {
        perror(TCBRED "[-] send" RESET);
        return 1;
    }

    return 0;
}

int recvMessage(int sock, char buffer[BUF_SIZE]) {
    memset(buffer, '\0', BUF_SIZE);
    if (recv(sock, buffer, BUF_SIZE, 0) < 0) {
        perror("[-] recv");
        return 1;
    }
    return 0;
}
