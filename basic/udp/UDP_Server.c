#include "udp_utils.h"

struct sockaddr_in receiveMessageFromClient(int sock) {
    struct sockaddr_in addr;

    char buffer[BUF_SIZE];
    addr = receiveMessage(sock, addr, buffer);
    printf("recvfrom: %s\n", buffer);

    return addr;
}

void sendMessageBackToClient(int sock, struct sockaddr_in client_addr) {
    char buffer[BUF_SIZE];
    memset(buffer, sizeof(char) * BUF_SIZE, '\0');
    strcpy(buffer, TCBBLU "THIS IS UDP SERVER" RESET);
    printf("sendto: %s\n", buffer);

    struct sockaddr_in _addr = sendMessage(sock, client_addr, buffer);
}

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    const int sock = createSocketUDP();
    struct sockaddr_in server_addr = getSocketAddress(ip, port);

    bindToPort(sock, server_addr);

    struct sockaddr_in client_addr = receiveMessageFromClient(sock);
    sendMessageBackToClient(sock, client_addr);

    closeSocketUDP(sock);

    return 0;
}