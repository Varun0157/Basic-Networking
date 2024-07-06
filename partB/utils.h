#ifndef __RECEIVE_H
#define __RECEIVE_H

#include "headers.h"

/*
Master function to for the receiver. Collects the sock-fd and the client_addr pointer
  (so further updates are not lost), and acts as the received. 
*/
void getData(int sock, struct sockaddr_in* client_addr);

/*
Receives the data and stores it in receivedData, performing required
  acknowledgements. 
*/
void receiveData(int sock, struct sockaddr_in* client_addr, int* numChunks,
                 char*** receivedData);
/*
Frees any globally assigned variables in receive.c
*/
void freeReceivedData(char** receivedData, int numChunks);

/*
Initialises globals in send.c, including for the creation of packets
*/
void initialiseGlobals(int numChunks, char* data);
/*
Frees the globals in send.c, that were dynamically allocated
*/
void freeGlobals(int numChunks);

/*
Communicates the number of chunks to be received and sent, terminating on failure. 
  This is because the spec sheet suggested that both client and server should be aware of
    the number of chunks
*/
int sendNumChunks(int sock, struct sockaddr_in addr, char* data);

/*
Sends a single packet corresponding to sequence number currSeqNum
*/
int sendPacket(int sock, struct sockaddr_in* addr, int currSeqNum, int numChunks);

/*
Gets the difference between end and start, in usecs
*/
long getTimeDiffInUs(struct timeval start, struct timeval end);

/*
Finds an unsent packet and returns its sequence number, or -1 if none
  are present
*/
int findUnsentPacket(int numChunks);
/*
Gets the wait time for re-transmission, in usecs
*/
long getWaitTime();

/*
Gets the ack corresponding to the given sequence number, printing success or fail 
  correspondingly. Has some special behaviour for the kill ack (-1)
*/
void getAck(int sock, struct sockaddr_in addr, int numChunks, int sentSeqNum,
            struct timeval startTime, int* receivedChunks);
/*
checks if any packets are present that have not been acknowledged despite sending, and 
  retransmits them if their timeout has been reached. 
*/
void retransmitPackets(int sock, struct sockaddr_in addr, int numChunks,
                       int* receivedChunks, bool killSeq);

/*
Finds acks that were received after the timeout expired. Thus, the packet is set up for
  re-transmission but no-longer needs to be anymore. 
More generally: collects acks that were received after the select() timeout expired, i.e., 
  did not arrive "immediately".
*/
void findLooseAcks(int sock, struct sockaddr_in addr, int* receivedChunks);

/*
master function for sender
*/
void sendData(int sock, struct sockaddr_in addr, void getDataFromFile(char**));

#endif
