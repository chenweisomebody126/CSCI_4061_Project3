#include <stdio.h>
#include <stdlib.h>
#include "mm.h"

/* Return usec */
double comp_time(struct timeval time_s, struct timeval time_e) {

  double elap = 0.0;

  if (time_e.tv_sec > time_s.tv_sec) {
    elap += (time_e.tv_sec - time_s.tv_sec - 1) * 1000000.0;
    elap += time_e.tv_usec + (1000000 - time_s.tv_usec);
  }
  else {
    elap = time_e.tv_usec - time_s.tv_usec;
  }
  return elap;
}

/*
TODO - Implement.  Return 0 for success, or -1 and set errno on fail.
allocate the memeory pool
allocate the memory stack consists of chunks (idx, ptr_mm_chunck)
*/
int mm_init(mm_t *mm, int hm, int sz) {
  if (mm != NULL){
    //initialize the pool of dynamic memeory
    mm->num_chunks = hm;
    mm->chunk_size =sz;
    mm->header = 0;
    /*
    Actual chunk consists of
    1) 4 bytes int indicating current chunk_idx
    2) 4 bytes int indicating next available chunk_idx
    3) memory of chunk_size to allocate
    */
    mm->act_chunk_size =sz+4*2;
    void* curr_chunk_ptr = mm->mm_pool;
    int chunk_idx= 0;
    for (chunk_idx; chunk_idx < mm->num_chunks ;chunk_idx++){
      //set the current chunk_idx
      *(int*) curr_chunk_ptr= chunk_idx;
      //set the next available chunk idx
      *(int*) (curr_chunk_ptr+4) = chunk_idx+1;
      curr_chunk_ptr =curr_chunk_ptr+ mm->act_chunk_size;
    }
    *(int*) (curr_chunk_ptr-mm->act_chunk_size +4) =-1;
    return 0;  /* TODO - return the right value */
  }
  else{
    fprintf(stderr, "The size of memory allocation requested is larger than %d bytes\n", CHUNK_SIZE*NUM_CHUNKS);
    exit(-1);
  }
}

/*
TODO - Implement to get a chunk of memory (pointer to void), NULL on failure
*/
void *mm_get(mm_t *mm) {
  //no more memory available to allocate
  if (mm->header==-1 ){
    fprintf(stderr, "No more memory available to allocate\n");
    return NULL;
  }
  //obtain the next available chunk and reset the header
  void* alloc_chunk = mm->mm_pool + mm->act_chunk_size*mm->header+4*2;
  mm->header = *(int*) (mm->mm_pool+ mm->act_chunk_size*mm->header+4);
  return alloc_chunk;
}

/*
TODO - Implement to give back ‘chunk’ to the memory manager, don’t free it though!
*/
void mm_put(mm_t *mm, void *chunk) {
  //"insert" received chunk, then update received chunk's next linked chunk as well as the header
  *(int*) (chunk-4) =  mm->header;
  mm->header = *(int*) (chunk-8);
}

/*
TODO - Implement to release all memory back to the system
*/
void mm_release(mm_t *mm) {
  free(mm->mm_pool);
  free(mm);
}

/*
 * TODO - This is just an example of how to use the timer.  Notice that
 * this function is not included in mm_public.h, and it is defined as static,
 * so you cannot call it from other files.  Instead, just follow this model
 * and implement your own timing code where you need it.
 */
static void timer_example() {
  struct timeval time_s, time_e;

  /* start timer */
  gettimeofday (&time_s, NULL);

  /* TODO - code you wish to time goes here */

  gettimeofday(&time_e, NULL);

  fprintf(stderr, "Time taken = %f msec\n",
          comp_time(time_s, time_e) / 1000.0);
}
