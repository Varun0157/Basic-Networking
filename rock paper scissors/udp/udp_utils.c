#include "udp_utils.h"

void closeSocketUDP(int socket) {
    if (closeSocket(socket) == 0) printf(TCBGRN "[+] socket closed\n " RESET);
}

int createSocketUDP() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror(TCBRED "[-]Socket Error" RESET);
        exit(1);
    }

    printf(TCBGRN "[+]UDP Server Socket created.\n" RESET);
    return sock;
}

int bindToPort(int sock, struct sockaddr_in server_addr) {
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(TCBRED "[-]Bind Error" RESET);
        return 1;
    }

    return 0;
}

int receiveMessage(int sock, struct sockaddr_in* addr, char buffer[BUF_SIZE]) {
    memset(buffer, '\0', BUF_SIZE);
    socklen_t addr_size = sizeof(addr);
    if (recvfrom(sock, buffer, BUF_SIZE, 0, addr, &addr_size) < 0) {
        perror(TCBRED "recvfrom" RESET);
        exit(1);
    }
}

int sendMessage(int sock, struct sockaddr_in* addr, char buffer[BUF_SIZE]) {
    if (sendto(sock, buffer, BUF_SIZE, 0, addr, sizeof(addr)) < 0) {
        perror(TCBRED "sendto" RESET);
        return 1;
    }

    return 0;
}
