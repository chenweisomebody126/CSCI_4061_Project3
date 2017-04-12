#include <stdio.h>

#include "mm.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>


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

/* TODO - Implement.  Return 0 for success, or -1 and set errno on fail. */
int mm_init(mm_t *mm, int hm, int sz) {
   int i;	//to allocate each chunk od size sz
//Initializing the size and number of chunks for the memory manager
   mm->size_of_chunks = sz;
   mm->number_of_chunks = hm;
   mm->count = 0;	//Initialize counter that keeps track of the number of used chunks
//Check if memory allocation to each chunk is successful
   if ((mm->memory_ptr= (void**)malloc(mm->number_of_chunks*sizeof(void*)))==NULL){
     fprintf(stderr, "%s\n", strerror(errno));
     return -1;	//-1 on error on malloc
   }
   for(i=0;i<hm;i++){
	 if ((mm->memory_ptr[i] = (void*)malloc(sz))==NULL){
      //error creating memory SET ERRNO
      fprintf(stderr, "%s\n", strerror(errno));
      return -1;	//error on malloc returns -1
    }
  }
	return 0;
}

void *mm_get(mm_t *mm) {
  void *temp;	//place holder to return the top most value to be used 
  if (mm->count<mm->number_of_chunks){
    temp = mm->memory_ptr[mm->count];
    mm->count++;
    return temp;	
  }
  return NULL;	//could not allocate as the number of chinks limit was reached
}

void mm_put(mm_t *mm, void *chunk) {
  if (mm->count<0){	//Particular chunk cannot be put back
    fprintf(stderr,"No such chunk to put back!\n");
    exit(1);
  }
  mm->count--;	//Chunk to be put back successfully, decreement counter tracking usage
  mm->memory_ptr[mm->count]=chunk;	
}


void mm_release(mm_t *mm) {
  void *temp;
  int i;
  for(i=0;i<mm->number_of_chunks-1;i++){	//free the total number of chunks 
    temp = mm->memory_ptr[i];
    free(temp);
  }
  free(mm->memory_ptr);	//free the memory pointer which was dynamically allocated
  mm->size_of_chunks = 0;
  mm->number_of_chunks = 0;
  mm->count = 0;
}
