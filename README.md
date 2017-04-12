 /* CSCI4061 S2017 Assignment 3 * login: patte5392umn.edu
* date: 4/12/17 * section: 1  
* name: Christopher Patterson, Sushmitha Ayyanar, Wei Chen
* id: patte539, ayyan003, chen4626 */

||Part A||
———————————————
1.
———————————————
2.
———————————————
3.
———————————————
4.


||Part B||
———————————————
1. How to compile
-Navigate to ./PATH/Project3/PartB
-run Makefile using “make” command in terminal
	Can also use “make clean” to clear and then remake using “make” if any problems
———————————————
2. How to run syntax: *NOTE* Sender and receiver must be run in different terminal windows!
Navigate to ./PATH/Project3/PartB
*Sender: packet_sender <num of messages to send> 
*Receiver: packet_receiver <num of messages to receive> 
———————————————  
3. Program description:
*Sender: The sender establishes a connection with a receiver using message queues and the subsequently transfers a number of messages (specified by user) to that receiver using packets. Messages can be of various lengths.
*Receiver: The receiver establishes a connection with a sender using message queues and then waits for the sender to initiate message transfers using packets. The packets for a given message are not assumed to come in order. The packets are assembled and the receiver displays all the messages it receives to the terminal as they come in. 
———————————————  
4. Contributions:
Wei handled coding and error handling for sender and Chris handled coding and error handling for receiver. The memory manager used was from PartA.

