
all:  packet_sender packet_receiver
	echo 'Made Sender and Receiver'

packet_sender: mm.o packet.h packet_sender.c
	gcc -o packet_sender packet_sender.c mm.o


packet_receiver: mm.o packet.h packet_receiver.c
	gcc -o packet_receiver packet_receiver.c mm.o

mm.o: mm.c mm.h
	gcc -o mm.o -c mm.c


clean:
	rm -rf packet_receiver packet_sender mm.o
