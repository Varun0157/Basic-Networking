#ifndef __UDP_UTILS_H
#define __UDP_UTILS_H

#include "../networking_utils.h"

int createSocketUDP();

void bindToPort(int sock, struct sockaddr_in server_addr);
struct sockaddr_in receiveMessage(int sock, struct sockaddr_in addr,
                                  char buffer[BUF_SIZE]);
struct sockaddr_in sendMessage(int sock, struct sockaddr_in client_addr,
                               char buffer[BUF_SIZE]);

void closeSocketUDP(int socket);

#endif
