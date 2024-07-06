#include "utils.h"

int getPort(char* portStr) {
    char* endPtr;
    long port = strtol(portStr, &endPtr, 10);
    if (*endPtr != '\0' || port < 0 || port > USHRT_MAX) {
        fprintf(stderr, "port %s must be an integer in [0, %d]\n" RESET, portStr,
                USHRT_MAX);
        exit(1);
    }

    return (int)port;
}

struct sockaddr_in getSocketAddress(const char* ip, const int port) {
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    return addr;
}

int closeSocket(int sock) {
    if (close(sock) >= 0) return 0;

    perror("[-] close");
    return 1;
}
