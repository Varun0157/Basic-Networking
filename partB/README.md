# Working

## General Overview
- Client reads `client_send.txt`, packets it into chunks of size 128 and sends it to the server. Once the client (sender in this case) has all of the data acknowledged, it sends a kill signal to the receiver, and receiver (server) aggregates it and appends to `received_data.txt`. 
  - Note that the kill signal is also retransmitted on failure. 
  - Server only terminates on receipt of a kill signal, sent by the client after all data has been acknowledged, and if no packet has been received for atleast `2*WAIT_TIME + 0.001 seconds`. This was an arbitrarily decided timeout. We have to set some kind of timeout as we do not actually know when the sender is done communicating, but we want to send data from the server too. 
- Next, server reads `server_send.txt`, packets it and sends it to the client, same as above. Client aggregates it and sends it to `received_data.txt`, same as above. The receiver and sender implementations are same for both client and address, stored in `send.c` and `receive.c`.

### send.c
Responsible for implementing the sender. 
- Try to send the number of chunks, terminate if failed after 5 seconds, because this MUST be communicated. 
- Get data and form packets
- While all the data has not been acknowledged:
  - Find any unsent packets and send them. 
  - In `getAck`: wait for the packet, if it does not arrive, set that item as `ackFail` and store the time. 
  - Acknowledge any received ack. 
- Make sure to find any "loose" acks throught, i.e., acks that were not sent by the receiver immediately but were sent after the timeout of `select`. 
- Check the packets that are in the `ackFail` state, and if the time since their sending has exceeded the timeout, re-transmit. 

### receive.c
Responsible for implementing the receiver. 
- Receive the number of chunks, and send it back to the sender to ensure correct amount was received. Sender terminates if this is incorrect. 
- Keep receiving data until a kill signal is received:
  - Accept a packet, terminate if it does not follow the expected sequence number. 
  - Send an ack with the same sequence number
- Once a kill is received, we only have to focus on receiving kill signals, the reset are effectively ignored. Terminate the server if no packet is received for `2*WAIT_TIME + 0.001 seconds`, as we assume no more packets are left to tbe sent in this case. 

### Client
- Send data, short sleep, receive data

### Server
- Receive data, short sleep, send data

### Why a kill signal? 
Simply keeping a count of ACKs sent out by the receiver is improper as we are using UDP. It is possible that some of these ACKs are lost. Thus, the sender will keep retransmitting some packet while the server thinks it is done and will break out. 
My code accounts for most simple edge cases (that I tried), commenting out different portions of receive allow us to test different complex scenarios. 

# Assumptions
- Number of chunks MUST immediately be communicated so both the receiver and sender know the expected number of distinct chunks. This is the only blocking message. 
  - _Advantage_: this actually lets us know if the receiver is ready, and prevents an infinite loop in case it is not. 
- It is not possible to flush a socket https://copyprogramming.com/howto/flushing-the-socket-streams-in-c-socket-programming. Thus, there are cases where we get improper output if sockets are improperly closed. 
- A "loose" ack is one that is not immediately received but still was sent by the server, so is received later than expected, by my definition. I'm just calling it this so I can differentiate from acks that are not immediately received. Eg: if we send acks after a fixed time greater than waiting period, or greater than timeout of select(). 
- The receiver terminates after not receiving any packets for time `WAIT_TIME + 0.001 seconds`, as we assume there are no more packets left to receive in this case. Overly slow communication may cause a break because of this.  
_All the test cases discussed in the spec-sheet work as intended, however_
