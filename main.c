// Run using `gcc -O3 -mavx512f -mavx512bw -mavx512vl bench.c strlen.o -o bench`

#define _POSIX_C_SOURCE 199309L
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern size_t strlen(const char *str);

#define ESCAPE(ptr) __asm__ volatile("" : : "g"(ptr) : "memory")
#define COMPILER_BARRIER() __asm__ volatile("" ::: "memory")

// large enough to spill out of Last Level Cache (LLC)
#define BUFFER_SIZE (0x100 * 0x400 * 0x400)

int main(void) {
  char *s = malloc(BUFFER_SIZE);
  if (!s) {
    printf("Memory allocation failed\n");
    return 1;
  }

  memset(s, 'A', BUFFER_SIZE - 1);
  s[BUFFER_SIZE - 1] = '\0';

  const int WARMUP_ITERS = 0x1000;
  const int ITERATIONS = 0x1000000;

  size_t dummy_sum = 0;
  for (int i = 0; i < WARMUP_ITERS; i++) {
    ESCAPE(s);
    dummy_sum += strlen(s);
    COMPILER_BARRIER();
  }

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < ITERATIONS; i++) {
    ESCAPE(s);
    dummy_sum += strlen(s);
    COMPILER_BARRIER();
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  double time_spent =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("String length verified: %zu\n",
         dummy_sum / (WARMUP_ITERS + ITERATIONS));
  printf("Time taken: %f seconds\n", time_spent);
  printf("Throughput: %.2f GB/s\n",
         ((double)ITERATIONS * BUFFER_SIZE) / time_spent / 1e9);

  free(s);
  return 0;
}
