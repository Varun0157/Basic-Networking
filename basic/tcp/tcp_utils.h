#ifndef __TCP_UTILS_H
#define __TCP_UTILS_H

#include "../utils.h"

void closeClientSocket(int sock);

int createSocketTCP();
void sendMessage(int socket, char buffer[BUF_SIZE]);
void recvMessage(int socket, char buffer[BUF_SIZE]);

#endif 
