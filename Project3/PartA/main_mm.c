#include "mm.h"
#include "stdio.h"
#include "stdlib.h"
void *arr[NUM_CHUNKS];

//function to keep track of the time for the implemented memory manager
static void timer_example_ours() {
  struct timeval time_s, time_e;
  int i;
  mm_t mm_partA;
  mm_init(&mm_partA,NUM_CHUNKS,CHUNK_SIZE);
  gettimeofday (&time_s, NULL);	//start timer

  for (i=0;i<NUM_CHUNKS;i++){

    arr[i]=mm_get(&mm_partA);	//get the memory chunks
  }
  for (i=0;i<NUM_CHUNKS;i++){
    mm_put(&mm_partA,arr[i]);	//put back the chunks
  }

  gettimeofday(&time_e, NULL);	//end timer

  mm_release(&mm_partA);
  fprintf(stderr, "Time taken by mm = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
}
static void timer_example_malloc() {
  struct timeval time_s, time_e;
  int i;
  /* start timer */
  gettimeofday (&time_s, NULL);	//start timer
  for (i=0;i<NUM_CHUNKS;i++){
    arr[i]=malloc(CHUNK_SIZE);	//malooc the chunks
  }
  for (i=0;i<NUM_CHUNKS;i++){
    free(arr[i]);	//free the chunks
  }

  gettimeofday(&time_e, NULL);	//end timer

  fprintf(stderr, "Time taken by malloc = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
}

int main(int argc, char **argv) {

  timer_example_ours();
  timer_example_malloc();
  return 0;
}
