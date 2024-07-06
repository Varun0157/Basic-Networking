#include "../utils.h"

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror(TCBRED "[-] socket " RESET);
        exit(1);
    }

    printf(TCBGRN "[+] TCP Server Socket created\n" RESET);
    printf(TCNGRN "port selected: %d\n" RESET, port);

    struct sockaddr_in addr = getSocketAddress(ip, port);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror(TCBRED "[-] connect" RESET);
        closeSocket(sock);
        exit(1);
    }

    printf(TCBGRN "[+] connected to the server\n" RESET);

    char buffer[BUF_SIZE];

    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "message from TCP client\n" RESET);
    printf("send: %s", buffer);
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror(TCBRED "[-] send" RESET);
        closeSocket(sock);
        exit(1);
    }

    memset(buffer, '\0', BUF_SIZE);
    if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
        perror("recv");
        closeSocket(sock);
        exit(1);
    }
    printf("recv: %s\n", buffer);

    closeSocket(sock);

    return 0;
}
