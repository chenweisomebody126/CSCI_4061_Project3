 /* CSCI4061 S2017 Assignment 3 * login: patte5392umn.edu
* date: 4/12/17 * section: 1  
* name: Christopher Patterson, Sushmitha Ayyanar, Wei Chen
* id: patte539, ayyan003, chen4626 */

||Part A||

1.Compiling:
-Navigate to: ./Project3/PartA
-For PartA, there is a Makefile that can be used. 
Run “make clean” to clear the executables. And run “make” to create the excecutables.

2.Running from the shell
The name of the executable for PartA, after make is done, is generated as test.
The following command:
./test
will execute the executable to produce the output timing values.

The executable generating command in make file is as shown here:
 gcc -o test main_mm.c mm.o
Run the executable:
./test

3.Program Description:
In this project, we mainly work with dynamic memory allocation and delocation. High overhead due to system calls for dynamic allocation/deallocation and because of the fact that malloc may not be thread-safe or signal-handler safe, we implement our own memory manager. We use mm_init function to dynamically allocate a certain number of chunks of a particular size (usually the case in real programs), mm_get to receive the chunks, that is, the chunks are in use, mm_put to put back to the memory manager making the chunks available for future use and mm_release to free all the memory allocated in mm_init. 
We further test and compare the memory manager with the case of directly using malloc, and we find that our implementation overcomes the problems with malloc

4.Work Distribution:
Sushmitha - mm_init, mm_get, mm_put and the timer functions
Christopher - mm_release, idea of using a double pointer for stack implementation

||Part B||
 
1. How to compile
-Navigate to ./PATH/Project3/PartB
-Run Makefile using “make” command in terminal
	Can also use “make clean” to clear and then remake using “make” if any problems
 
2. How to run syntax: *NOTE* Sender and receiver must be run in different terminal windows!
Navigate to ./PATH/Project3/PartB
*Sender: packet_sender <num of messages to send> 
*Receiver: packet_receiver <num of messages to receive> 
   
3. Program description:
*Sender: The sender establishes a connection with a receiver using message queues and the subsequently transfers a number of messages (specified by user) to that receiver using packets. Messages can be of various lengths.
*Receiver: The receiver establishes a connection with a sender using message queues and then waits for the sender to initiate message transfers using packets. The packets for a given message are not assumed to come in order. The packets are assembled and the receiver displays all the messages it receives to the terminal as they come in. 
   
4. Contributions:
Wei handled coding and error handling for sender and Chris handled coding and error handling for receiver. The memory manager used was from PartA.

