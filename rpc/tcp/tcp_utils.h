#ifndef __TCP_UTILS_H
#define __TCP_UTILS_H

#include "../utils.h"

void closeClientSocket(int sock);

int createSocketTCP();
int sendMessage(int socket, char buffer[BUF_SIZE]);
int recvMessage(int socket, char buffer[BUF_SIZE]);

#endif
