#ifndef __UTILS_H
#define __UTILS_H

#include "headers.h"

#define BUF_SIZE 1024
#define IP_ADDR "127.0.0.1"

int closeSocket(int sock);

int getPort(char* portStr);
int getPorts(int argc, const char** argv);
struct sockaddr_in getSocketAddress(const char* ip, const int port);

#endif