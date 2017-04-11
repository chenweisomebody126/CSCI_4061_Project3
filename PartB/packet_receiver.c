
#include "packet.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int msqid = -1;

static message_t message;   /* current message structure */
static mm_t mm;             /* memory manager will allocate memory for packets */
static int pkt_cnt = 0;     /* how many packets have arrived for current message */
static int pkt_total = 1;   /* how many packets to be received for the message */

/*
   Handles the incoming packet.
   Store the packet in a chunk from memory manager.
   The packets for given message will come out of order.
   Hence you need to take care to store and assemble it correctly.
   Example, message "aaabbb" can come as bbb -> aaa, hence, you need to assemble it
   as aaabbb.
   Hint: "which" field in the packet will be useful.
 */
static void packet_handler(int sig) {
  packet_t pkt;
  void *chunk;
  packet_queue_msg msg_pkt;

 /*Get chunk from memory manager*/
  if ((chunk = mm_get(&mm))==NULL){
      fprintf(stderr, "Failed to get memory chunk from memory manager: %s\n", strerror(errno));
  }

/*get the "packet_queue_msg" from the queue.*/
  if((msgrcv(msqid, (void*)chunk, sizeof(packet_queue_msg), QUEUE_MSG_TYPE, 0))==-1){
    fprintf(stderr, "Failed to receive packet from message queue: %s\n", strerror(errno));
    exit(-1);
  }
/*extract the packet from "packet_queue_msg" and store it in the
memory from memory manager. Use memcpy() to store tpacket in chunk*/
  pkt =((packet_queue_msg*) chunk)->pkt;
  pkt_total = pkt.how_many;
  pkt_cnt++;
  message.num_packets = pkt_cnt;
  memcpy(chunk, &pkt, sizeof(packet_t));
  message.data[pkt.which] = chunk;
}

/*
 * Create message from packets ... deallocate packets.
 * Return a pointer to the message on success, or NULL
 */
static char *assemble_message() {

  char *msg;
  int i;
  packet_t *temp_pkt;
  int msg_len = message.num_packets * sizeof(data_t);

  /* Allocate msg and assemble packets into it:
  Malloc used to initialize message - failed if NULL
  memcpy() used to assemble message, NUL char added
  to end of message. */
  if ((msg = (char*)malloc(sizeof(char) * (msg_len+1)))==NULL){
    fprintf(stderr, "Malloc failed to initialize msg: %s\n", strerror(errno));
  }
  for (i=0; i<message.num_packets; i++){
    temp_pkt = message.data[i];
    memcpy(msg+i*sizeof(data_t), temp_pkt->data, sizeof(data_t));
  }
  msg[msg_len]='\0';

  /* reset these for next message */
  pkt_total = 1;
  pkt_cnt = 0;
  message.num_packets = 0;

  return msg;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: packet_sender <num of messages to receive>\n");
    exit(-1);
  }

  int k = atoi(argv[1]); /* no of messages you will get from the sender */
  int i;
  char *msg;

  /*init memory manager for NUM_CHUNKS chunks of size CHUNK_SIZE each */
  mm_init(&mm, NUM_CHUNKS, CHUNK_SIZE);
  message.num_packets = 0;

  /*TODO - Solutions have send pid fail if sender isn't already running!!??
  initialize msqid to send pid and receive messages from the message queue. Use the key in packet.h */
  if ((msqid = msgget(key, IPC_CREAT | 0666))==-1){
    //Couldn't create message queue or get identifier
    fprintf(stderr, "Failed to get message queue: %s\n", strerror(errno));
    exit(-1);
  }

  /* send process pid to the sender on the queue */
  pid_queue_msg receiver_pid;
  receiver_pid.mtype = QUEUE_MSG_TYPE;
  receiver_pid.pid = getpid();
  fprintf(stderr, "Sending pid: %d\n", receiver_pid.pid);
  if ((msgsnd(msqid, (void*)&receiver_pid, sizeof(pid_queue_msg),0666))==-1){
    //Error sending pid to the sender
    fprintf(stderr,"Failed to send pid to sender: %s\n", strerror(errno));
    exit(-1);
  }

  /*set up SIGIO handler to read incoming packets from the queue. Check packet_handler()*/
  struct sigaction act;
  act.sa_handler = packet_handler;
  sigfillset(&act.sa_mask);
  if (sigaction(SIGIO, &act, NULL)==-1){
    fprintf(stderr, "Failed to set SIGIO: %s\n", strerror(errno));
  }

  for (i = 1; i <= k; i++) {
    while (pkt_cnt < pkt_total) {
      pause(); /* block until next packet */
    }

    msg = assemble_message();
    if (msg == NULL) {
      perror("Failed to assemble message");
    }
    else {
      fprintf(stderr, "GOT IT: message=%s\n", msg);
      free(msg);
    }
  }

  // TODO deallocate memory manager
  mm_release(&mm);
  // TODO remove the queue once done
  if (msgctl(msqid, IPC_RMID, NULL)==-1){
    fprintf(stderr, "Failed to remove message queue: %s\n", strerror(errno));
  }
  return EXIT_SUCCESS;
}
