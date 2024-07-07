#include "headers.h"

void freeReceivedData(char** receivedData, int numChunks) {
    for (int i = 0; i < numChunks; i++) free(receivedData[i]);
    free(receivedData);
}

void receiveData(int sock, struct sockaddr_in* client_addr, int* numChunks,
                 char*** receivedData) {
    char buffer[CHUNK_SIZE];
    memset(buffer, '\0', CHUNK_SIZE);
    socklen_t addr_size = sizeof(client_addr);
    if (recvfrom(sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)client_addr, &addr_size) <
        0) {
        perror(TCBRED "recvfrom" RESET);
        close(sock);
        exit(1);
    }
    *numChunks = atoi(buffer);
    printf(TCBBLU "[+]Number of Chunks: %d\n" RESET, *numChunks);

    // sending it back to confirm receipt.
    if (sendto(sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)client_addr, addr_size) <
        0) {
        perror("sendto");
        close(sock);
        exit(1);
    }

    *receivedData = (char**)malloc(sizeof(char*) * (*numChunks));
    for (int i = 0; i < *numChunks; i++) {
        (*receivedData)[i] = (char*)malloc(sizeof(char) * CHUNK_SIZE + 1);
        memset((*receivedData)[i], '\0', CHUNK_SIZE + 1);
    }

    int numProcessed = 0;
    bool killReceived = false;
    while (!killReceived) {
        numProcessed += 1;

        Packet packet;
        addr_size = sizeof(client_addr);
        if (recvfrom(sock, &packet, sizeof(packet), 0, (struct sockaddr*)client_addr,
                     &addr_size) < 0) {
            perror(TCBRED "recvfrom" RESET);
            close(sock);
            freeReceivedData(*receivedData, *numChunks);
            exit(1);
        }
        printf(TCBGRN "%d Received\n" RESET, packet.seqNum);
        if (packet.seqNum >= *numChunks || packet.seqNum < 0) {
            printf(TCBRED "Invalid Sequence number received, terminating\n" RESET);
            close(sock);
            freeReceivedData(*receivedData, *numChunks);
            exit(1);
        }
        if (packet.kill) {
            killReceived = true;
            continue;
        }

        // this is a test to show power of loose acks. Set wait time as 3 (anything > 2)
        // and add this if, before the next set of code (so it continues from an else if
        // instead of if)
        // if(numProcessed == 1){
        //   sleep(2);
        //   ACK ack;
        //   ack.seqNum = packet.seqNum;
        //   if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr,
        //   addr_size)
        //   <
        //       0) {
        //     perror(TCBRED "sendto" RESET);
        //     close(sock);
        //     freeReceivedData(*receivedData, *numChunks);
        //     exit(1);
        //   }
        //   printf(TCBGRN "%d ACK Sent\n" RESET, packet.seqNum);
        // } else

        // if (numProcessed % 3 != 0) {
        //   ACK ack;
        //   ack.seqNum = packet.seqNum;
        //   if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr,
        //   addr_size)
        //   <
        //       0) {
        //     perror(TCBRED "sendto" RESET);
        //     close(sock);
        //     freeReceivedData(*receivedData, *numChunks);
        //     exit(1);
        //   }
        //   printf(TCBGRN "%d ACK Sent\n" RESET, packet.seqNum);
        // } else {
        //   printf(TCBRED "%d ACK Not Sent\n" RESET, packet.seqNum);
        //   continue;
        // }

        // sleep(1);
        ACK ack;
        ack.seqNum = packet.seqNum;

        if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr, addr_size) <
            0) {
            perror(TCBRED "sendto" RESET);
            close(sock);
            exit(1);
        }
        printf(TCBGRN "%d ACK Sent\n" RESET, packet.seqNum);
        strncpy((*receivedData)[packet.seqNum], packet.data, CHUNK_SIZE);
    }

    printf(TCBBLU "Kill received, sending ACK bit -1 and breaking\n" RESET);
    // comment out this first send to ensure retransmitted kill ack is received and
    // working
    ACK ack;
    ack.seqNum = -1;
    if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr, addr_size) <
        0) {
        perror("sendto");
        close(sock);
        exit(1);
    }

    while (true) {
        bool found = false;
        socklen_t addr_size;

        fd_set readFDs;
        FD_ZERO(&readFDs);
        FD_SET(sock, &readFDs);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 2 * getWaitTime() + 1000;

        int activity = select(sock + 2, &readFDs, NULL, NULL, &timeout);
        if (activity < 0) {
            perror(TCBRED "select" RESET);
            close(sock);
            exit(1);
        } else if (activity == 0) {
            // timeout
        } else {
            Packet packet;
            addr_size = sizeof(client_addr);
            if (recvfrom(sock, &packet, sizeof(packet), 0, (struct sockaddr*)client_addr,
                         &addr_size) < 0) {
                perror(TCBRED "recvfrom" RESET);
                close(sock);
                freeReceivedData(*receivedData, *numChunks);
                exit(1);
            }

            if (packet.kill) {
                printf(
                    TCBBLU
                    "Kill received again, sending acknowledgement and breaking\n" RESET);
                ACK ack;
                ack.seqNum = -1;
                if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr,
                           addr_size) < 0) {
                    perror("sendto");
                    close(sock);
                    exit(1);
                }
            }

            found = true;
        }

        if (!found) break;
    }

    printf(TCBCYN
           "Receiver: Ending after %ld usec as we did not receive a packet\n" RESET,
           2 * getWaitTime() + 1000);
}

void getData(int sock, struct sockaddr_in* client_addr) {
    printf(TCBYEL "Listening...\n" RESET);

    int numChunks;
    char** receivedData;
    receiveData(sock, client_addr, &numChunks, &receivedData);

    char aggregatedData[CHUNK_SIZE * (numChunks + 1)];
    memset(aggregatedData, '\0', CHUNK_SIZE * (numChunks + 1));
    for (int i = 0; i < numChunks; i++) {
        strcat(aggregatedData, receivedData[i]);
    }
    aggregatedData[strlen(aggregatedData)] = '\0';

    printf("Aggregated Data:\n\n %s\n", aggregatedData);

    FILE* fd = fopen("received_data.txt", "a");
    if (!fd) {
        printf(TCBRED "Error opening file, cannot write to it\n" RESET);
        freeReceivedData(receivedData, numChunks);
        close(sock);
        exit(1);
    }
    fwrite(aggregatedData, sizeof(char), strlen(aggregatedData), fd);
    fclose(fd);

    freeReceivedData(receivedData, numChunks);
}
