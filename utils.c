#include "utils.h"

int getPort(int argc, const char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    char* endPtr;
    long port = strtol(argv[1], &endPtr, 10);
    if (*endPtr != '\0' || port < 0 || port > USHRT_MAX) {
        fprintf(stderr, "port %s must be an integer in [0, %d]\n" RESET, argv[1],
                USHRT_MAX);
        exit(1);
    }

    return (int)port;
}