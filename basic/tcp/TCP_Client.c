#include "tcp_utils.h"

void connectToServer(int sock, struct sockaddr_in addr) {
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror(TCBRED "[-] connect" RESET);
        closeClientSocket(sock);
        exit(1);
    }
    printf(TCBGRN "[+] connected to the server\n" RESET);
}

void sendMessageToServer(int sock) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "message from TCP client\n" RESET);
    printf("send: %s", buffer);

    sendMessage(sock, buffer);
}

void receiveMessageFromServer(int sock) {
    char buffer[BUF_SIZE];
    recvMessage(sock, buffer);
    printf("recv: %s\n", buffer);
}

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    const int sock = createSocketTCP();
    struct sockaddr_in addr = getSocketAddress(ip, port);

    connectToServer(sock, addr);

    sendMessageToServer(sock);
    receiveMessageFromServer(sock);

    closeClientSocket(sock);

    return 0;
}
