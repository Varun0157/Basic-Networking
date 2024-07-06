#include "../game_utils.h"
#include "udp_utils.h"

void closeSockets(int sock1, int sock2) {
    if (sock1 >= 0 && close(sock1) == 0) printf(TCBGRN "[+] Socket 1 closed\n" RESET);
    if (sock2 >= 0 && close(sock2) == 0) printf(TCBGRN "[+] Socket 2 closed\n" RESET);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }

    const char* ip = "127.0.0.1";
    const int port1 = getPort(argv[1]), port2 = getPort(argv[2]);

    const int sock1 = createSocketUDP(), sock2 = createSocketUDP();
    printf(TCBGRN "[+]UDP Server Sockets created.\n" RESET);
    printf(TCNGRN "Ports selected: %d and %d\n" RESET, port1, port2);

    struct sockaddr_in server_addr1 = getSocketAddress(ip, port1),
                       server_addr2 = getSocketAddress(ip, port2);

    if (bindToPort(sock1, server_addr1) != 0) {
        closeSockets(sock1, sock2);
        exit(1);
    }
    if (bindToPort(sock2, server_addr2) != 0) {
        closeSockets(sock1, sock2);
        exit(1);
    }
    printf(TCNGRN "Listening...\n" RESET);

    char buffer[BUF_SIZE];

    int keepPlaying = 1;
    while (keepPlaying) {
        if (receiveMessage(sock1, &server_addr1, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        const int move1 = atoi(buffer);
        printf(TCBBLU "Client 1 played: %s\n" RESET, getMove(move1));

        if (receiveMessage(sock2, &server_addr2, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        const int move2 = atoi(buffer);
        printf(TCBBLU "Client 2 played: %s\n" RESET, getMove(move2));

        printf("%s\n", getServerResult(move1, move2));

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE,
                 "%s\nPlayer 2 played %s\nEnter 0 to stop, or 1 to continue: ",
                 getResult(move1, move2, 1), getMove(move2));
        if (sendMessage(sock1, &server_addr1, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE,
                 "%s\nPlayer 1 played %s\nEnter 0 to stop, or 1 to continue: ",
                 getResult(move1, move2, 2), getMove(move1));
        if (sendMessage(sock2, &server_addr2, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        if (receiveMessage(sock1, &server_addr1, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        const int cont1 = atoi(buffer);

        if (receiveMessage(sock2, &server_addr2, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        const int cont2 = atoi(buffer);

        memset(buffer, '\0', BUF_SIZE);
        keepPlaying = cont1 && cont2;
        snprintf(buffer, BUF_SIZE, "%d", keepPlaying ? 1 : 0);

        if (sendMessage(sock1, &server_addr1, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        if (sendMessage(sock2, &server_addr2, buffer) != 0) {
            closeSockets(sock1, sock2);
            exit(1);
        }

        printLines();
    }

    closeSockets(sock1, sock2);

    return 0;
}