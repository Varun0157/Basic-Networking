#ifndef __UDP_UTILS_H
#define __UDP_UTILS_H

#include "../networking_utils.h"

int createSocketUDP();

int bindToPort(int sock, struct sockaddr_in server_addr);
int receiveMessage(int sock, struct sockaddr_in* addr, char buffer[BUF_SIZE]);
int sendMessage(int sock, struct sockaddr_in* addr, char buffer[BUF_SIZE]);

void closeSocketUDP(int socket);

#endif
