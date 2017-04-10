
#include "packet.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int msqid = -1;
int msgflg = 06660; /* msgflg to be passed to msgget() */

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

  if ((chunk = mm_get(&mm))==NULL){
      fprintf(stderr, "Failed to get memory chunk from memory manager.\n");
  }


  // TODO get the "packet_queue_msg" from the queue.
  if(msgrcv(msgid, (void*)&msg_pkt, sizeof(packet_queue_msg), PKT_MSG_TYPE, 0))==-1){
    fprintf(stderr, "Failed to receive packet from message queue %s\n", strerr(errno));
    exit(-1);
  }

  // TODO extract the packet from "packet_queue_msg" and store it in the memory from memory manager
  pkt = msg_pkt.pkt;
  pkt_total = pkt.how_many;
  pkt_cnt++;
  message.num_packets = pkt_cnt;
  memcpy(chunk, (void)*pkt, sizeof(pkt));
  message.data[pkt.which] = chunk;
}

/*
 * TODO - Create message from packets ... deallocate packets.
 * Return a pointer to the message on success, or NULL
 */
static char *assemble_message() {

  char *msg;
  int i;
  packet_t temp_pkt;
  int msg_len = message.num_packets * sizeof(data_t);

  /* TODO - Allocate msg and assemble packets into it */
  msg = (char*)malloc(sizeof(char) * (msg_len+1));
  for (i=0; i<message.num_packets; i++){
    temp_pkt = message.data[i];
    memcpy(msg+i, temp_pkt.data, sizeof(temp_pkt.data));
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

  /* TODO - init memory manager for NUM_CHUNKS chunks of size CHUNK_SIZE each */
  mm_init(&mm, NUM_CHUNKS, CHUNK_SIZE);
  message.num_packets = 0;

  /* TODO initialize msqid to send pid and receive messages from the message queue. Use the key in packet.h */
  if ((msqid = msgget(key,msgflg|IPC_CREAT))<0){
    //Couldn't create message queue or get identifier
    fprintf(stderr, "Failed to create message queue or retrieve queue identifier. %s\n", strerr(errno));
    exit(-1);
  }
  /* TODO send process pid to the sender on the queue */
  pid_queue_msg receiver_pid;
  receiver_pid.mtype = QUEUE_MSG_TYPE;
  receiver_pid.pid = getpid();

  if ((msgsnd(msqid, (void*)&receiver_pid, sizeof(pid_queue_msg),msgflg))<0){
    //Error sending pid to the sender
    fprintf(stderr,"Failed to send reciever's pid to sender. %s\n", strerr(errno));
    exit(-1);
  }

  /* TODO set up SIGIO handler to read incoming packets from the queue. Check packet_handler()*/
  struct sigaction act;
  act.sa_handler = packet_handler;
  sigfillset(&act.sa_mask);
  if (sigaction(SIGIO, &act, NULL)<0){
    fprintf(stderr, "Failed to set SIGIO. %s\n", strerr(errno));
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
  if (msgctl(msqid, IPC_RMID, NULL)<0){
    fprintf(stderr, "Failed to remove message queue. %s\n", strerr(errno));
  }
  return EXIT_SUCCESS;
}
