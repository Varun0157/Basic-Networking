#include "tcp_utils.h"

void printLines() {
    for (int i = 0; i < 10; i++) printf("-");
    printf("\n");
}

char* getMove(int move) {
    if (move == 0)
        return "rock";
    else if (move == 1)
        return "paper";
    else
        return "scissors";
}

char* getResult(int move1, int move2, int client) {
    int res = (move1 - move2 + 3) % 3;
    if (res == 0) return "\033[1;33mDRAW!\033[0m";

    if (client == 1) {
        if (res == 1) {
            return "\033[1;32mVICTORY!\033[0m";
        } else {
            return "\033[1;31mLOSS!\033[0m";
        }
    } else {
        if (res == 1) {
            return "\033[1;31mLOSS!\033[0m";
        } else {
            return "\033[1;32mVICTORY!\033[0m";
        }
    }
}

char* getServerResult(int move1, int move2) {
    int res = (move1 - move2 + 3) % 3;
    if (res == 0)
        return "DRAW";
    else if (res == 1)
        return "CLIENT 1 WINS";
    else
        return "CLIENT 2 WINS";
}

void closeSockets(int server_socket1, int server_socket2, int client_sock1,
                  int client_sock2) {
    if (server_socket1 >= 0 && closeSocket(server_socket1) == 0)
        printf("[+] closed first server socket\n");
    if (server_socket2 >= 0 && closeSocket(server_socket2) == 0)
        printf("[+] closed second server socket\n");
    if (client_sock1 >= 0 && closeSocket(client_sock1) == 0)
        printf("[+] closed first client socket\n");
    if (client_sock2 >= 0 && closeSocket(client_sock2) == 0)
        printf("[+] closed second client socket\n");
}

void bindToPort(struct sockaddr_in addr, int socket, int port, int otherSocket) {
    if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        char error[BUF_SIZE];
        snprintf(error, BUF_SIZE, TCBRED "[-] bind error on port %d" RESET, port);
        perror(error);

        closeSockets(socket, otherSocket, -1, -1);
        exit(1);
    }
}

void listenForConnections(int port, int socket, int otherSocket) {
    if (listen(socket, 5) == -1) {
        char error[BUF_SIZE];
        snprintf(error, BUF_SIZE, TCBRED "[-] listen error on port %d" RESET, port);
        perror(error);

        closeSockets(socket, otherSocket, -1, -1);
        exit(1);
    }
}

int acceptClientConnection(int port, int serverSock, int otherSock) {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_sock;
    if ((client_sock = accept(serverSock, (struct sockaddr*)&client_addr, &addr_size)) ==
        -1) {
        char error[BUF_SIZE];
        snprintf(error, BUF_SIZE, TCBRED "[-] accept error on port %d" RESET, port);
        perror(error);

        closeSockets(serverSock, otherSock, -1, -1);
        exit(1);
    }

    return client_sock;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <port1> <port2>", argv[0]);
        exit(1);
    }

    const char* ip = IP_ADDR;
    const int port1 = getPort(argv[1]), port2 = getPort(argv[2]);

    const int server_sock1 = createSocketTCP(), server_sock2 = createSocketTCP();

    struct sockaddr_in server_addr1 = getSocketAddress(ip, port1),
                       server_addr2 = getSocketAddress(ip, port2);

    bindToPort(server_addr1, server_sock1, port1, server_sock2);
    bindToPort(server_addr2, server_sock2, port2, server_sock1);
    printf(TCBGRN "[+] bound to ports %d and %d\n" RESET, port1, port2);

    listenForConnections(port1, server_sock1, server_sock2);
    listenForConnections(port2, server_sock2, server_sock1);
    printf(TCBGRN "[+] listening for connections\n" RESET);

    char buffer[BUF_SIZE];

    const int client_sock1 = acceptClientConnection(port1, server_sock1, server_sock2);
    printf(TCBGRN "[+] client 1 Connected\n" RESET);

    const int client_sock2 = acceptClientConnection(port2, server_sock2, server_sock1);
    printf(TCBGRN "[+] client 2 Connected\n" RESET);

    int playGame = 1;
    while (playGame) {
        printLines();

        if (recvMessage(client_sock1, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        const int move1 = atoi(buffer);
        printf(TCBBLU "client 1 played: %s\n" RESET, getMove(move1));

        if (recvMessage(client_sock2, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        const int move2 = atoi(buffer);
        printf(TCBBLU "client 2 played: %s\n" RESET, getMove(move2));

        printf("%s\n", getServerResult(move1, move2));

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE,
                 "%s\nPlayer 2 played %s\nEnter 0 to stop, or 1 to continue: ",
                 getResult(move1, move2, 1), getMove(move2));
        if (sendMessage(client_sock1, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        memset(buffer, '\0', BUF_SIZE);
        snprintf(buffer, BUF_SIZE,
                 "%s\nPlayer 1 played %s\nEnter 0 to stop, or 1 to continue: ",
                 getResult(move1, move2, 2), getMove(move1));
        if (sendMessage(client_sock2, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        if (recvMessage(client_sock1, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }
        const int cont1 = atoi(buffer);

        if (recvMessage(client_sock2, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }
        const int cont2 = atoi(buffer);

        memset(buffer, '\0', BUF_SIZE);
        playGame = cont1 && cont2;
        snprintf(buffer, BUF_SIZE, "%d", playGame ? 1 : 0);

        if (sendMessage(client_sock1, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        if (sendMessage(client_sock2, buffer) == 1) {
            closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);
            exit(1);
        }

        printLines();
    }

    closeSockets(server_sock1, server_sock2, client_sock1, client_sock2);

    return 0;
}
