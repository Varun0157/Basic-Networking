### 1. 
1. [Source1: IBM Docs on TCP](https://www.ibm.com/docs/en/zos-basic-skills?topic=4-transmission-control-protocol-tcp)

2. [Source2: PacketLife](https://packetlife.net/blog/2010/jun/7/understanding-tcp-sequence-acknowledgment-numbers/)

##### Data Sequencing
When a host initiates a TCP connection, its initial sequence number is effectively random, however we start at seqNum 0. (Source: 2). 
The implementation of data-sequencing is not very different from traditional TCP, apart from the fact that traditional TCP keeps track of every byte sent outward by a host.(Source: 1) I, on the other hand, am dividing into CHUNK-SIZE bits, and so even though these chunks are accounted for, data may be lost within them. I do not guarantee as much as TCP does. If I defined CHUNK-SIZE as 1, then there would be no apparent difference. 

##### Re-transmissions
1. [Source 1: Baeldung](https://www.baeldung.com/cs/tcp-retransmission-rules)

The re-transmission rules in general are quite advanced in TCP. 
TCP Segments are said to be lost when:
- the time-out expires
- it receives three duplicate acknowledgements (Source 1)

In our TCP-esque implementation however, we only consider the first factor. 

2. [Source 2: Geant TCP](https://wiki.geant.org/display/public/EK/SelectiveAcknowledgements)
3. [Source 3: Wikipedia TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol#Selective_acknowledgments)

Additionally, traditional TCP used to emply cumulative acknowledgements which leads to the need to re-transmit large amounts of data (including data that might actually have been received!). Thus, there are inefficiencies when packets are lost.
 Our implementation aligns more with the modern-TCP method of _Selective Acknowledgement_, which allows a receiver to acknowledge non-consecutive data, so the sender can re-transmit only what is missing. 
___
___
### 2. 

1. [Source: Wikipedia - Flow Control in TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol#Flow_control)

The server can set a variable window size that it updates on receipt of data. 
_The sender can only send that amount of data before it must wait for an acknowledgement and receive a window update from the receiving host._
###### Sender Side
Thus, in my implementation, I would continue non-blocking sending of data (as I am currently doing) until that specific window size, and then carry out a blocked-wait for the window update from the receiver (i.e., without using select), on the receipt of which I update the amount of data I can send without waiting for the next update. 
I can set an overall max-time interval before which I terminate, using select again if required. 

###### Receiver Side
I would accept the data sent from the client upto the given window size set. Then, I would dynamically update the window size as required, based on the input received, and send out the updated window size for the client to receive. 

Trivially, if the window size becomes 0 the sender can no longer send data unless a new non-zero window size is provided on update. 

Thus, this effectively simulates flow-control. 
