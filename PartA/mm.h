#ifndef __MM_H
#define __MM_H

#include <sys/time.h>


#define INTERVAL 0
#define INTERVAL_USEC 50000
#define CHUNK_SIZE 64
#define NUM_CHUNKS 1000

/* TODO - Fill this in */
typedef struct {
  int *status; //False is free True is taken
  int size_of_chunks;
  int number_of_chunks;
  void *memory_ptr; //poiter to array of chunks
} mm_t;

/* TODO - Implement these in mm.c */
double comp_time(struct timeval time_s, struct timeval time_e);
int mm_init(mm_t *mm, int num_chunks, int chunk_size);
void *mm_get(mm_t *mm);
void mm_put(mm_t *mm, void *chunk);
void mm_release(mm_t *mm);


#endif
