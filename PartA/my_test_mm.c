#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "mm.h"

static int assert_that(int condition, char *label) {
  fprintf(stderr, "%-6s %s\n", condition ? "OK" : "NOT OK", label);
  return condition;
}

/**
 * Function argument for qsort.  We're given a pointer to two elements
 * in an array, and need to return -1, 0, or 1 if the first is smaller,
 * equal to, or larger than the second, respectively.
 */
static int compare_pointers(const void *a, const void *b) {
  /*
   * A bit awkward... parameters point to array elements which are void*
   * themselves.  Cast first to the real type, then deref to get the pointed-to
   * void*.
   */
  void *ap = *((void **) a);
  void *bp = *((void **) b);
  return ap < bp ? -1 : (ap > bp ? 1 : 0);
}

static int chunks_unique(void **array, size_t n) {
  size_t i;
  int ok;
  for (i = 1, ok = 1; ok && i < n; i++) {
    ok = array[i - 1] != array[i];
    if (!ok) {
      fprintf(stderr, "Same chunk (%p) returned at least twice\n", array[i]);
    }
  }
  return ok;
}

static int chunk_spacing_ok(void **array, size_t n, ptrdiff_t min_space) {
  size_t i;
  int ok;
  for (i = 1, ok = 1; ok && i < n; i++) {
    ptrdiff_t diff = (char *) array[i] - (char *) array[i - 1];
    ok = diff >= min_space;
    if (!ok) {
      fprintf(stderr,
              "Chunk %ld (%p) and %ld (%p) too close (%ld bytes diff)\n",
              i - 1,
              array[i - 1],
              i,
              array[i],
              diff);
    }
  }
  return ok;
}

/**
 * Simply check that allocation succeeds.  It should on Linux for
 * reasonable num_chunks and chunk_size values.
 */
static int can_initialize(int num_chunks, int chunk_size) {
  mm_t mm;
  int ret = mm_init(&mm, num_chunks, chunk_size);
  if (ret == 0) {
    mm_release(&mm);
  }
  return assert_that(ret == 0, "mm_init: can initialize");
}

/**
 * Check that allocation (mm_get) works correctly.
 *   - Right number before NULL return
 *   - Sufficient spacing
 *   - Unique chunks returned
 */
static int can_allocate(int num_chunks, int chunk_size) {
  mm_t mm;
  size_t i;
  int ok;
  int passed = 0;
  void **chunks = malloc(num_chunks * sizeof(void *));
  if (chunks == NULL) {
    perror("Unexpected: Failed to allocate chunks.  Skipping tests.");
    return passed;
  }

  if (mm_init(&mm, num_chunks, chunk_size) != 0) {
    perror("mm_init failed.  Skipping tests");
    free(chunks);
    return passed;
  }

  /* Allocate all chunks */
  ok = 1;
  for (i = 0; i < num_chunks; i++) {
    chunks[i] = mm_get(&mm);
    ok = chunks[i] != NULL;
  }
  passed += assert_that(ok, "mm_get: able to allocate all num_chunks chunks");

  /* Try another allocation */
  passed += assert_that(mm_get(&mm) == NULL, 
                        "mm_get: NULL returned when out of memory");

  qsort(chunks, num_chunks, sizeof(void *), compare_pointers);
  passed += assert_that(chunks_unique(chunks, num_chunks),
                        "mm_get: all chunks are unique");
  passed += assert_that(chunk_spacing_ok(chunks, num_chunks, chunk_size), 
                        "mm_get: chunks are spaced far enough apart");

  mm_release(&mm);
  free(chunks);

  return passed;
}

/**
 * Check that deallocation (mm_put) works correctly.
 *   - All chunks can be reused
 *   - Single deallocated chunk gets re-used
 */
static int can_deallocate(int num_chunks, int chunk_size) {
  mm_t mm;
  size_t i;
  int cmp, passed = 0;
  void **first_chunks = malloc(num_chunks * sizeof(void *));
  void **second_chunks = malloc(num_chunks * sizeof(void *));
  if (first_chunks == NULL || second_chunks == NULL) {
    perror("Unexpected: Failed to allocate {first,second}_chunks.  "
           "Skipping tests.");
    free(first_chunks);
    free(second_chunks);
    return passed;
  }

  if (mm_init(&mm, num_chunks, chunk_size) != 0) {
    perror("mm_init failed.  Skipping tests");
    free(first_chunks);
    free(second_chunks);
    return passed;
  }

  /* Allocate all chunks */
  for (i = 0; i < num_chunks; i++) {
    first_chunks[i] = mm_get(&mm);
  }

  /* Free all chunks */
  for (i = 0; i < num_chunks; i++) {
    mm_put(&mm, first_chunks[i]);
  }

  /* Allocate all again */
  for (i = 0; i < num_chunks; i++) {
    second_chunks[i] = mm_get(&mm);
  }

  /*
   * Since order doesn't matter, sort both, then compare to be sure that the
   * allocator worked with the same chunks both times.
   */
  qsort(first_chunks, num_chunks, sizeof(void *), compare_pointers);
  qsort(second_chunks, num_chunks, sizeof(void *), compare_pointers);
  cmp = memcmp(first_chunks, second_chunks, num_chunks * sizeof(void *));
  passed += assert_that(cmp == 0, "mm_put: all freed chunks reused");

  /* Now free just one, and check that mm_get returns it again.
   * First for a chunk at the beginning. */
  mm_put(&mm, first_chunks[0]);
  passed += assert_that(mm_get(&mm) == first_chunks[0],
                        "mm_put: lone freed chunk returned on next get (beginning)");

  /* Then at the middle */
  mm_put(&mm, first_chunks[num_chunks / 2]);
  passed += assert_that(mm_get(&mm) == first_chunks[num_chunks / 2],
                        "mm_put: lone freed chunk returned on next get (middle)");

  /* And at the end */
  mm_put(&mm, first_chunks[num_chunks - 1]);
  passed += assert_that(mm_get(&mm) == first_chunks[num_chunks - 1],
                        "mm_put: lone freed chunk returned on next get (end)");

  mm_release(&mm);
  free(first_chunks);
  free(second_chunks);

  return passed;
}

/*
 * Run a series of very simple tests to check the correctness of a
 * fixed-chunk-size memory allocator.
 */
int main(int argc, char **argv) {

  int sizes[] = {1, 4, 15, 1024};
  int nums[] = {1, 4, 500, 9999};

  int init_passed = 0;
  int get_passed = 0;
  int put_passed = 0;

  int i, j, chunk_size, num_chunks, init_ok;
  for (i = 0; i < sizeof sizes / sizeof sizes[0]; i++) {
    for (j = 0; j < sizeof nums / sizeof nums[0]; j++) {
      chunk_size = sizes[i];
      num_chunks = nums[j];
      printf("--- chunk_size=%d, num_chunks=%d ---\n", chunk_size, num_chunks);

      init_ok = can_initialize(num_chunks, chunk_size);
      init_passed += init_ok;
      
      if (init_ok) {
        get_passed += can_allocate(num_chunks, chunk_size);
        put_passed += can_deallocate(num_chunks, chunk_size);
      }

    }
  }

  printf("--- Results ---\n");
  printf("  mm_init:    passed %d tests\n", init_passed);
  printf("  mm_get:     passed %d tests\n", get_passed);
  printf("  mm_put:     passed %d tests\n", put_passed);
  printf("  mm_release: no tests\n");

  return EXIT_SUCCESS;
}
