#include "udp_utils.h"

struct sockaddr_in sendMessageToServer(int sock, struct sockaddr_in addr) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "THIS IS UDP CLIENT" RESET);
    printf("sendto: %s\n", buffer);

    return sendMessage(sock, addr, buffer);
}

void receiveMessageFromServer(int sock, struct sockaddr_in addr) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    struct sockaddr_in _addr = receiveMessage(sock, addr, buffer);

    printf("recvfrom: %s\n", buffer);
}

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    char buffer[BUF_SIZE];

    const int sock = createSocketUDP();
    struct sockaddr_in client_addr = getSocketAddress(ip, port);

    client_addr = sendMessageToServer(sock, client_addr);
    receiveMessageFromServer(sock, client_addr);

    closeSocketUDP(sock);

    return 0;
}