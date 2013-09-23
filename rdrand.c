#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpuid.h"

#include <argp.h>

#include "arguments.h"

#ifndef bit_RDRAND
#define bit_RDRAND 0x40000000
#endif

int cpu_supports_rdrand() {
  uint32_t eax=0, ebx=0, ecx=0, edx=0;
  __get_cpuid(1, &eax, &ebx, &ecx, &edx);
  return ecx & bit_RDRAND;
}

int rdrand_fill_array(uint64_t* array, int size) {
  // We want to do this as quickly as possible. Assembly hoy!
  uint64_t successes = 0;
  uint64_t temp = 0;

  __asm volatile(
      "top_of_rdrand_loop%=:"
      "jecxz end_of_rdrand_loop%= ;\n"   // jump if ecx (size) == 0

      "rdrand %1 ;\n"                    // Generate random value
      "adc $0, %0 ;\n"                   // Check if successul

      "mov %1, (%2) ;\n "                // Store value in array
      "add $8, %2 ;\n "                  // Move array to next spot

      "dec %4 ;\n"                       // Decrement size
      "jmp top_of_rdrand_loop%= ;\n"

      "end_of_rdrand_loop%=:\n"
    : "=r" (successes), "=r" (temp), "=r"(array)
    : "0" (successes), "c" (size), "2"(array)
    );

  return (int)successes;
}

int main(int argc, char** argv) {
  struct arguments arguments;
  arguments.block_count = 16;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if(!cpu_supports_rdrand()) {
    printf("Error: Your CPU does not support RDRAND.\n");
    exit(1);
  }

  int size = 0;
  int blocks_left = arguments.block_count;
  int round_size = 0;
#define MAX_ROUND_SIZE 16
  uint64_t array[MAX_ROUND_SIZE];

  while(blocks_left > 0) {
    round_size = (blocks_left > MAX_ROUND_SIZE) ? MAX_ROUND_SIZE : blocks_left;

    size = rdrand_fill_array(array, round_size);

    if(size != round_size) {
      printf("rdrand round unsuccessful: %d != %d!\n", size, round_size);
    } else {
      printf("rdrand successful: %d\n", size);

      for(int i = 0; i < size; i++) {
        printf("%" PRIx64 "\n", array[i]);
      }
      blocks_left -= round_size;
    }
  }

  return 0;
}
