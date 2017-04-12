#include "mm.h"
#include "stdio.h"
#include "stdlib.h"
static void timer_example_ours() {
  struct timeval time_s, time_e;
  void *arr[NUM_CHUNKS];
  int i;
  /* start timer */
  //gettimeofday (&time_s, NULL);
  mm_t mm_partA;
  //mm_partA = (mm_t *)malloc(sizeof(mm_t));
  mm_init(&mm_partA,NUM_CHUNKS,CHUNK_SIZE);
  gettimeofday (&time_s, NULL);
  for (i=0;i<NUM_CHUNKS;i++){
    arr[i]=mm_get(mm_partA);
  }
  for (i=0;i<NUM_CHUNKS;i++){
    mm_put(mm_partA,arr[i]);
  }

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken by mm = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
free(mm_partA->memory_ptr);
//free(arr);
}
static void timer_example_malloc() {
  struct timeval time_s, time_e;
  void *arr[NUM_CHUNKS];
  int i;
  /* start timer */
  gettimeofday (&time_s, NULL);
  for (i=0;i<NUM_CHUNKS;i++){
    arr[i]=malloc(CHUNK_SIZE);
  }
  for (i=0;i<NUM_CHUNKS;i++){
    free(arr[i]);
  }

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken by malloc = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
}

int main(int argc, char **argv) {
  timer_example_ours();
//  free(mm->memory_ptr);
  timer_example_malloc();
  return 0;
}