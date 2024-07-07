#include "networking_utils.h"

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

void setDataToSend(char** data, char* filePath) {
    FILE* fd = fopen(filePath, "r");
    if (!fd) {
        printf(TCBRED "Unable to get the data to send\n" RESET);
        exit(1);
    }
    fseek(fd, 0, SEEK_END);
    const long numChars = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    *data = (char*)malloc(sizeof(char) * (numChars + 1));
    size_t len = fread(*data, sizeof(char), numChars, fd);
    (*data)[len] = '\0';
    fclose(fd);
}
