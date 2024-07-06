#include "../game_utils.h"
#include "udp_utils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    const char* ip = "127.0.0.1";
    const int port = getPort(argv[1]);

    const int sock = createSocketUDP();
    struct sockaddr_in addr = getSocketAddress(ip, port);

    char buffer[BUF_SIZE];
    while (1) {
        printLines();

        const int input = getUserInput();

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE, "%d", input);
        printf(TCBBLU "Client: %s\n" RESET, buffer);
        addr = sendMessage(sock, addr, buffer);

        addr = receiveMessage(sock, addr, buffer);
        printf(TCBYEL "Server: %s" RESET, buffer);

        int keepPlaying;
        scanf("%d", &keepPlaying);

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE, "%d", keepPlaying == 0 ? 0 : 1);
        printf(TCBBLU "Client: %s\n" RESET, buffer);
        addr = sendMessage(sock, addr, buffer);

        addr = receiveMessage(sock, addr, buffer);
        const int continuePlaying = buffer[0] == '0' ? 0 : 1;
        printf("Server: %s\n", !continuePlaying ? "End" : "Continue");
        printLines();

        if (!continuePlaying) break;
    }

    close(sock);

    return 0;
}