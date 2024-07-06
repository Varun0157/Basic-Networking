#include "tcp_utils.h"

void closeSockets(int clientSock, int serverSock) {
    if (serverSock >= 0 && closeSocket(clientSock) == 0)
        printf(TCBGRN "[+] client socket closed\n" RESET);
    if (serverSock >= 0 && closeSocket(serverSock) == 0)
        printf(TCBGRN "[+] server socket closed\n" RESET);
}

void bindToPort(int server_sock, struct sockaddr_in server_addr) {
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(TCBRED "[-]Bind Error" RESET);
        closeSockets(-1, server_sock);
        exit(1);
    }

    printf(TCBGRN "[+] bound to port" RESET);
}

void listenForConnections(int server_sock) {
    const int backlog = 5;

    if (listen(server_sock, backlog) == -1) {
        perror(TCBRED "[-] listen" RESET);
        closeSockets(-1, server_sock);
        exit(1);
    }
    printf(TCBGRN "Listening...\n" RESET);
}

int acceptClientConnection(int serverSock) {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_sock;
    if ((client_sock = accept(serverSock, (struct sockaddr*)&client_addr, &addr_size)) ==
        -1) {
        perror(TCBRED "accept" RESET);
        closeSockets(-1, serverSock);
        exit(1);
    }
    printf(TCBGRN "[+] client connected\n" RESET);

    return client_sock;
}

void receiveMessageFromClient(int clientSock, int serverSock) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    if (recv(clientSock, buffer, sizeof(buffer), 0) == -1) {
        perror("[-] recv");
        closeSockets(clientSock, serverSock);
        exit(1);
    }
    printf("recv: %s", buffer);
}

void sendMessageToClient(int clientSock, int serverSock) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "message from TCP server" RESET);
    printf("send: %s\n", buffer);
    if (send(clientSock, buffer, strlen(buffer), 0) == -1) {
        perror("[-] send");
        closeSockets(clientSock, serverSock);
        exit(1);
    }
}

int main(int argc, const char** argv) {
    const char* ip = IP_ADDR;
    const int port = getPort(argc, argv);

    const int server_sock = createSocketTCP();
    struct sockaddr_in server_addr = getSocketAddress(ip, port);

    bindToPort(server_sock, server_addr);
    listenForConnections(server_sock);
    const int client_sock = acceptClientConnection(server_sock);

    receiveMessageFromClient(client_sock, server_sock);
    sendMessageToClient(client_sock, server_sock);

    closeSockets(client_sock, server_sock);

    return 0;
}
