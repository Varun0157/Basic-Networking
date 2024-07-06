#include "headers.h"

/*
Use of select() in recvfrom for nonblocking. Idea came from this:
https://stackoverflow.com/a/15941974
https://www.ibm.com/docs/en/zos/2.5.0?topic=calls-select

select() call monitors activity on a set of sockets looking for sockets ready for reading/
writing etc int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
struct timeval *timeout)
nfds -> number of socket descriptors to be checked. Val should be gretaer than greatest
number of sockets to be checked. sock + 2 because a sock for server may be opened later if
we open client first maybe. readfds -> fds to check for reading. Same for writefds,
exceptfds timeout -> timeout to wait for fd to complete.

Am using this instead of non-blocking assignment. Setting a very low timeout for select to
receive ack.
*/

/*
General Idea:
- Try to send number of chunks, if fail: exit
  - Assumption: number of chunks MUST be received within 5 seconds, or we terminate. This data
    MUST be communicated for us to continue. 
- get data and form packets
- while not all data has been acknowledged
  - find any "loose" acks (i.e., acks that may have been sent after we moved on). Example:
we wait for very short time, but what if the ack comes later? This will collect it later.
    ASSUMPTION: race condition of this delayed ack coming at same time as us sending data
is not considered.
  - Find any unsent packets, send them. In getAck:
      - Wait for packet, if does not arrive, set that item as ackFail and store time.
      - It is retransmitted by a later function if the time exceeds the wait time. Note
that if it arrives between these two scenarios, then it will be collected by the
getLooseAcks, and we will no longer have to retransmit. This is how the wait time is
respected.
      - Acknowledge ANY ack that is received.
  - findLooseAcks again.
  - Retransmit any acks whose time is more than wait time and still not received.
- Next, send a kill signal to the receiver so it knows all the data has been acknowledged
  and we will not be sending any more packets. Repeatedly send a packet with a kill bit set, 
  until we receive a -1 ack, denoting receipt of the kill message. Then, we terminate. 

Assumption: Short time intervals may lead to conditions of sending and receiving at the same time, 
  this is expected to break the implementation. 
*/

/*
Stores the input data in packets, indexed by sequence number
*/
Packet* packets;
/*
Checks the `status` of the sequence number.
*/
status* received;
/*
For `ackFail` items, stores the time the signal was sent, to determine when to
re-transmit.
*/
struct timeval* timeSent;

/*
Initialises packets, received, timeSent
*/
void initialiseGlobals(int numChunks, char* data) {
  packets = (Packet*)malloc(sizeof(Packet) * numChunks);
  received = (status*)malloc(sizeof(status) * numChunks);
  timeSent = (struct timeval*)malloc(sizeof(struct timeval) * (numChunks));

  for (int i = 0; i < numChunks; i++) {
    strncpy(packets[i].data, data + i * CHUNK_SIZE, CHUNK_SIZE);
    packets[i].data[CHUNK_SIZE] = '\0';
    packets[i].seqNum = i;
    packets[i].kill = false;

    received[i] = notSent;
  }
}

/*
Frees globals defined in send.c
*/
void freeGlobals(int numChunks) {
  free(packets);
  free(received);
  free(timeSent);
}

/*
Attempts to communicate the number of chunks to receive. If this fails, then terminate.
*/
int sendNumChunks(int sock, struct sockaddr_in addr, char* data) {
  int dataSize = strlen(data);
  int numChunks =
      dataSize % CHUNK_SIZE ? dataSize / CHUNK_SIZE + 1 : dataSize / CHUNK_SIZE;

  char buffer[CHUNK_SIZE];
  memset(buffer, '\0', CHUNK_SIZE);
  snprintf(buffer, CHUNK_SIZE, "%d", numChunks);
  if (sendto(sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("sendto");
    close(sock);
    free(data);
    exit(1);
  }
  printf(TCBGRN "[+] Number of Chunks Sent: %s\n\n" RESET, buffer);

  fd_set readFDs;
  FD_ZERO(&readFDs);
  FD_SET(sock, &readFDs);

  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  int activity = select(sock + 2, &readFDs, NULL, NULL, &timeout);
  if (activity < 0) {
    perror(TCBRED "select" RESET);
    close(sock);
    exit(1);
  } else if (activity == 0) {
    // timeout
    printf(TCBRED "Acknowledgement for chunks not received\n" RESET);
    close(sock);
    exit(1);
  } else {
    memset(buffer, '\0', CHUNK_SIZE);
    socklen_t addr_size = sizeof(addr);
    if (recvfrom(sock, buffer, CHUNK_SIZE, 0, (struct sockaddr*)&addr, &addr_size) < 0) {
      perror(TCBRED "recvfrom" RESET);
      close(sock);
      exit(1);
    }

    printf(TCBYEL "[+] Chunks Acknowledged: %s\n" RESET, buffer);
    int recvedChunks = atoi(buffer);
    if (recvedChunks != numChunks) {
      printf(TCBRED "Incorrect ack received for chunks\n" RESET);
      close(sock);
      exit(1);
    }

    return numChunks;
  }

  return -1;
}

/*
Send the packet with the given sequence number
*/
int sendPacket(int sock, struct sockaddr_in* addr, int currSeqNum, int numChunks) {
  if (sendto(sock, &(packets[currSeqNum]), sizeof(Packet), 0, (struct sockaddr*)addr,
             sizeof(*addr)) < 0) {
    perror("sendto");
    close(sock);
    freeGlobals(numChunks);
    exit(1);
  }

  printf(TCBBLU "[+] %d Sent\n" RESET, packets[currSeqNum].seqNum);
  return currSeqNum;
}

long getTimeDiffInUs(struct timeval start, struct timeval end) {
  return (end.tv_usec - start.tv_usec) + ((int)1e6 * (end.tv_sec - start.tv_sec));
}

int findUnsentPacket(int numChunks) {
  for (int i = 0; i < numChunks; i++) {
    if (received[i] == notSent) {
      return i;
    }
  }
  return -1;
}

long getWaitTime() { return (int)((double)(1e6) * WAIT_TIME); }

/*
Gets the ack for the current packet
*/
void getAck(int sock, struct sockaddr_in addr, int numChunks, int sentSeqNum,
            struct timeval startTime, int* receivedChunks) {
  socklen_t addr_size;

  bool ackReceived = false;

  fd_set readFDs;
  FD_ZERO(&readFDs);
  FD_SET(sock, &readFDs);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 1000;

  int activity = select(sock + 2, &readFDs, NULL, NULL, &timeout);
  if (activity < 0) {
    perror(TCBRED "select" RESET);
    close(sock);
    exit(1);
  } else if (activity == 0) {
    // timeout
  } else {
    ACK ack;
    addr_size = sizeof(addr);
    if (recvfrom(sock, &ack, sizeof(ACK), 0, (struct sockaddr*)&addr, &addr_size) < 0) {
      perror(TCBRED "recvfrom" RESET);
      close(sock);
      exit(1);
    }

    if (ack.seqNum != sentSeqNum) {
      ackReceived = false;

      if (sentSeqNum >= 0 && ack.seqNum >= 0 && received[ack.seqNum] != ackSuccess) {
        received[ack.seqNum] = ackSuccess;
        printf(TCBGRN "[+] %d: Ack Received\n" RESET, ack.seqNum);
        *receivedChunks += 1;
      }
    } else {
      ackReceived = true;
    }
  }

  if (ackReceived) {
    if (sentSeqNum >= 0) {
      received[sentSeqNum] = ackSuccess;
    }
    printf(TCBGRN "[+] %d: Ack Received\n" RESET, sentSeqNum);
    *receivedChunks += 1;
  } else {
    if (sentSeqNum >= 0) {
      received[sentSeqNum] = ackFail;
      timeSent[sentSeqNum] = startTime;
    }
    printf(TCBRED "[+] %d: Ack not Received\n" RESET, sentSeqNum);
  }
}

void retransmitPackets(int sock, struct sockaddr_in addr, int numChunks,
                       int* receivedChunks, bool killSeq) {
  for (int s = 0; s < numChunks; s++) {
    if (received[s] == ackFail) {
      struct timeval currTime;
      if (gettimeofday(&currTime, NULL) < 0) {
        perror(TCBRED "gettimeofday" RESET);
        exit(1);
      }
      long timeDiff = getTimeDiffInUs(timeSent[s], currTime);

      if (timeDiff > getWaitTime()) {
        printf("Retr: ");
        int retransmittedSeq = sendPacket(sock, &addr, s, numChunks);
        if (killSeq) {
          getAck(sock, addr, numChunks, -1, currTime, receivedChunks);
        } else {
          getAck(sock, addr, numChunks, retransmittedSeq, currTime, receivedChunks);
        }
      }
    }
  }
}

/*
Acks that were not immediately received, but still may be received later.
*/
void findLooseAcks(int sock, struct sockaddr_in addr, int* receivedChunks) {
  while (true) {
    bool found = false;
    socklen_t addr_size;

    fd_set readFDs;
    FD_ZERO(&readFDs);
    FD_SET(sock, &readFDs);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    int activity = select(sock + 2, &readFDs, NULL, NULL, &timeout);
    if (activity < 0) {
      perror(TCBRED "select" RESET);
      close(sock);
      exit(1);
    } else if (activity == 0) {
      // timeout
    } else {
      ACK ack;
      addr_size = sizeof(addr);
      if (recvfrom(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&addr, &addr_size) < 0) {
        perror("recvfrom");
        close(sock);
        exit(1);
      }

      found = true;
      if (packets[0].kill && ack.seqNum < 0) {
        printf(TCBGRN "%d: Kill Ack Received\n" RESET, ack.seqNum);
        received[0] = ackSuccess;
        *receivedChunks += 1;
      } else if (!packets[0].kill && ack.seqNum >= 0 &&
                 received[ack.seqNum] != ackSuccess) {
        printf(TCBGRN "%d: Loose Ack Received\n" RESET, ack.seqNum);
        received[ack.seqNum] = ackSuccess;
        *receivedChunks += 1;
      }
    }

    if (!found) break;
  }
}

void sendData(int sock, struct sockaddr_in addr, void getDataFromFile(char**)) {
  char* data;
  getDataFromFile(&data);

  int numChunks = sendNumChunks(sock, addr, data);
  initialiseGlobals(numChunks, data);
  free(data);

  int receivedChunks = 0;
  while (receivedChunks < numChunks) {
    struct timeval startTime;
    if (gettimeofday(&startTime, NULL) < 0) {
      perror(TCBRED "gettimeofday" RESET);
      exit(1);
    }
    int packNum;
    if ((packNum = findUnsentPacket(numChunks)) >= 0) {
      int sentSeqNum = sendPacket(sock, &addr, packNum, numChunks);
      getAck(sock, addr, numChunks, sentSeqNum, startTime, &receivedChunks);
    }

    findLooseAcks(sock, addr, &receivedChunks);
    retransmitPackets(sock, addr, numChunks, &receivedChunks, false);
    findLooseAcks(sock, addr, &receivedChunks);
  }

  printf(TCBCYN "Setting kill signal in packet 0\n" RESET);
  packets[0].kill = true;
  received[0] = notSent;
  receivedChunks -= 1;
  while (receivedChunks < numChunks) {
    struct timeval startTime;
    if (gettimeofday(&startTime, NULL) < 0) {
      perror(TCBRED "gettimeofday" RESET);
      exit(1);
    }
    int packNum;
    if ((packNum = findUnsentPacket(numChunks)) >= 0) {
      sendPacket(sock, &addr, packNum, numChunks);
      getAck(sock, addr, numChunks, -1, startTime, &receivedChunks);
    }

    findLooseAcks(sock, addr, &receivedChunks);
    retransmitPackets(sock, addr, numChunks, &receivedChunks, true);
    findLooseAcks(sock, addr, &receivedChunks);
  }

  freeGlobals(numChunks);
}