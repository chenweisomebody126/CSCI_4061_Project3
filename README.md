# CSCI_4061_Project3 
 ##Sender:   
  gcc -o rec packet_receiver.c mm.o      
  ipcrm -Q 4061      //(optional: to remove the message queue)       
 ./send 3        
 
##Recever:    
 gcc -o rec packet_receiver.c mm.o      
 ./rec 3    

