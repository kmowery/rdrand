#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpuid.h"

#ifndef bit_RDRAND
#define bit_RDRAND 0x40000000
#endif

int cpu_supports_rdrand() {
  uint32_t eax, ebx, ecx, edx;
  __get_cpuid(1, &eax, &ebx, &ecx, &edx);
  return ecx & bit_RDRAND;
}

int rdrand_fill_array(uint64_t* array, int size) {
  // We want to do this as quickly as possible. Assembly hoy!
  uint64_t successes = 0;
  uint64_t temp = 0;

  __asm volatile(
      "top_of_rdrand_loop:"
      // "jump if ecx == 0"
      "jecxz end_of_rdrand_loop ;"

      "rdrand %1 ;"
      "adc $0, %0 ;"

      "mov %1, (%4) ; "
      "add $8, %4 ; "

      "dec %3 ;"
      "jmp top_of_rdrand_loop ;"

      "end_of_rdrand_loop:"
    : "=r" (successes), "=r" (temp)
    : "0" (successes), "c" (size), "r"(array)
    );

  return (int)successes;
}

int main(int argc, char** argv) {
  if(!cpu_supports_rdrand()) {
    printf("Error: Your CPU does not support RDRAND.\n");
    exit(1);
  }

  int size = 0;
  uint64_t array[16];

  size = rdrand_fill_array(array, 16);

  printf("rdrand successful: %d\n", size);

  for(int i = 0; i < size; i++) {
    printf("%" PRIx64 "\n", array[i]);
  }

  return 0;
}
