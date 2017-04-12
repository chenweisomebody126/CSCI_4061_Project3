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
  int i;
   mm->size_of_chunks = sz;
	 mm->number_of_chunks = hm;
   mm->count = 0;
   if ((mm->memory_ptr= (void**)malloc(mm->number_of_chunks*sizeof(void*)))==NULL){
     fprintf(stderr, "%s\n", strerror(errno));
     return -1;
   }
   for(i=0;i<hm;i++){
	 if ((mm->memory_ptr[i] = (void*)malloc(sz))==NULL){
      //error creating memory SET ERRNO
      fprintf(stderr, "%s\n", strerror(errno));
      return -1;
    }
  }
	return 0;
}
  /*If hm*sz size memory dynamically created sucessfully:
  assign status and memory array to the memory manager*/


void *mm_get(mm_t *mm) {
  void *temp;
  if (mm->count<mm->number_of_chunks){
    temp = mm->memory_ptr[mm->count];
    mm->count++;
    return temp;
  }
  return NULL;
}

void mm_put(mm_t *mm, void *chunk) {
  if (mm->count<0){
    fprintf(stderr,"No such chunk to put back!\n");
    exit(1);
  }
  mm->count--;
  mm->memory_ptr[mm->count]=chunk;
}


void mm_release(mm_t *mm) {
  void *temp;
  int i;
  for(i=0;i<mm->number_of_chunks-1;i++){
    temp = mm->memory_ptr[i];
    free(temp);
  }
  free(mm->memory_ptr);
  mm->size_of_chunks = 0;
  mm->number_of_chunks = 0;
  mm->count = 0;
}
