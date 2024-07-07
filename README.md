# Networking

A set of 3 independent projects to gain a foundation on networking concepts. 

## 1. basic

An implementation of a simple client-server model for both TCP and UDP protocols.

### Instructions

```bash
cd basic
make
```
In one terminal, run the server
```bash
./tcp_server <port>
```
In another, run the client 
```bash
./tcp_client <port>
```
Replace `tcp` with `udp` in the above steps for the udp implementation. 

### Implementation 
Steps are abstracted greatly to break up the functioning into concrete steps. For example, the `main` function of TCP_Client is, roughly:
```
sock = createSocket()
addr = getAddress(ip, port)
connectToServer(sock, addr)
sendMessageToServer(sock)
receiveMessageFromServer(sock)

closeSocket(sock)
```

The code for `tcp` and `udp` are contained in the directories of the same name. `tdp_utils.c` and `udp_utils.c` contain functionality common to both the client and the server (sending and receiving messages, creating sockets, etc.). 

`utils` in the main folder contains functionality common to both tcp and udp: getting port data from the command line, getting the address struct from the ip and port, etc. 

## 2. rock paper scissors
This showcases a simple use case of the client-server model. It performs the following:
- start a server which listens for two clients on two different ports
- start two clients
- users enter their decisions on the client end (rock, paper, scissors)
- server receives the decision, deliberates, and informs the clients of the results
- if both clients say they wish to keep playing, the game continues, else, it terminates. 

The clients can play in any order. Both `tcp` and `udp` models have been implemented. 

### Instructions

```bash
cd rock\ paper\ scissors
make
```
In one terminal, run the server
```bash
./tcp_server < port1 > < port2 >
```
In others, run the two clients 
```bash
./tcp_client < port1 / port2 >
```

## 3. TCP From Scratch
Implementing _some_ `TCP` functionality from "scratch", using `UDP`. The functionalities implemented are:
1. **Data Sequencing**: the sender divides the data into smaller chunks of fixed size, each of which is assigned  number. After the receiver has all the data, it is aggregated in the correct order. 
2. **Retransmissions**: The receiver must send an ACK packet for every data chunk received. If the sender doesn't receive the acknowledgement for a chunk within a reasonable amount of time, it resends the data. 

