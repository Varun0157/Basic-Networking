#include "../utils.h"

void closeSockets(int clientSock, int serverSock) {
    if (serverSock >= 0 && closeSocket(clientSock) >= 0)
        printf(TCBGRN "[+] client socket closed\n" RESET);
    if (serverSock >= 0 && closeSocket(serverSock) >= 0)
        printf(TCBGRN "[+] server socket closed\n" RESET);
}

int main(int argc, const char** argv) {
    const int port = getPort(argc, argv);
    const char* ip = IP_ADDR;

    const int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror(TCBRED "[-] socket " RESET);
        exit(1);
    }
    printf(TCBGRN "[+] TCP Server Socket created.\n" RESET);
    printf(TCNGRN "port selected: %d\n" RESET, port);

    struct sockaddr_in server_addr = getSocketAddress(ip, port);
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(TCBRED "[-]Bind Error" RESET);
        closeSockets(-1, server_sock);
        exit(1);
    }

    printf(TCBGRN "[+] bound to port" RESET);

    if (listen(server_sock, 5) == -1) {
        perror(TCBRED "[-] listen" RESET);
        closeSockets(-1, server_sock);
        exit(1);
    }

    printf(TCBGRN "Listening...\n" RESET);

    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_sock;
    if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size)) ==
        -1) {
        perror(TCBRED "accept" RESET);
        closeSockets(-1, server_sock);
        exit(1);
    }
    printf(TCBGRN "[+] client connected\n" RESET);

    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    if (recv(client_sock, buffer, sizeof(buffer), 0) == -1) {
        perror("[-] recv");
        closeSockets(client_sock, server_sock);
        exit(1);
    }
    printf("recv: %s", buffer);

    memset(buffer, '\0', BUF_SIZE);
    strcpy(buffer, TCBBLU "message from TCP server" RESET);
    printf("send: %s\n", buffer);
    if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
        perror("[-] send");
        closeSockets(client_sock, server_sock);
        exit(1);
    }

    return 0;
}
