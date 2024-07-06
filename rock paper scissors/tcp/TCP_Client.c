#include "../game_utils.h"
#include "tcp_utils.h"

void connectToServer(int sock, struct sockaddr_in addr) {
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror(TCBRED "[-] connect" RESET);
        closeClientSocket(sock);
        exit(1);
    }
    printf(TCBGRN "[+] connected to the server\n" RESET);
}

void closeClientSocket(int sock) {
    if (closeSocket(sock) == 0) printf(TCBGRN "[+] socket closed\n" RESET);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    const char* ip = IP_ADDR;
    const int port = getPort(argv[1]);

    const int sock = createSocketTCP();
    struct sockaddr_in addr = getSocketAddress(ip, port);

    connectToServer(sock, addr);

    char buffer[BUF_SIZE];
    while (1) {
        printLines();

        const int input = getUserInput();

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE, "%d", input);
        printf(TCBBLU "Client: %s\n" RESET, buffer);
        if (sendMessage(sock, buffer) == -1) {
            closeClientSocket(sock);
            exit(1);
        }

        if (recvMessage(sock, buffer) == -1) {
            closeClientSocket(sock);
            exit(1);
        }
        printf(TCBCYN "Server: %s" RESET, buffer);

        int keepPlaying;
        scanf("%d", &keepPlaying);

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE, "%d", keepPlaying == 0 ? 0 : 1);
        printf(TCBBLU "Client: %s\n" RESET, buffer);
        if (sendMessage(sock, buffer) == -1) {
            closeClientSocket(sock);
            exit(1);
        }

        if (recvMessage(sock, buffer) == -1) {
            closeClientSocket(sock);
            exit(1);
        }
        const int continuePlaying = buffer[0] == '0' ? 0 : 1;
        printf(TCBYEL "Server: %s\n" RESET, !continuePlaying ? "End" : "Continue");

        printLines();

        if (buffer[0] == '0') break;
    }

    closeClientSocket(sock);

    return 0;
}
