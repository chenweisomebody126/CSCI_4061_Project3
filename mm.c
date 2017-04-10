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

  int statuses[hm];
	 mm->size_of_chunks = sz;
	 mm->number_of_chunks = hm;
	 mm->status = statuses;
	for(i=0;i<hm;i++)
		mm->status[i]=0;
	if ((mm->memory_ptr  = (void*)malloc(mm->number_of_chunks * mm->size_of_chunks))==NULL){
    //error creating memory SET ERRNO
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }
	return 0;



  }
  /*If hm*sz size memory dynamically created sucessfully:
  assign status and memory array to the memory manager*/
  //mm->memory_ptr = Allocated Memory;
  //mm->status= statuses;
  //return 0;
//}


void *mm_get(mm_t *mm) {
  int i;
  /*Search the status of each chunk to find next free chunk*/
  for (i = 0; i < mm->number_of_chunks; i++) {
      if (mm->status[i]==0){
        mm->status[i]=1;
        return mm->memory_ptr+(i*mm->size_of_chunks);
      }
  }
  return NULL;
}

void mm_put(mm_t *mm, void *chunk) {
  int i;
  //int chunk_address = chunk;
  int found = 0;
  for (i=0; i<(mm->number_of_chunks); i++){
    if (chunk == mm->memory_ptr+(i*mm->size_of_chunks)){
      mm->status[i] = 0;
      found = 1;
      break;
    }
  }
  //Mismatched address. Chunk not found!
  if (!found){
    fprintf(stderr, "Chunk at address  not found!");
  }


}


void mm_release(mm_t *mm) {
  free(mm->memory_ptr);
  mm->status  = NULL;
  mm->size_of_chunks = 0;
  mm->number_of_chunks = 0;
}
