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
  arguments.block_count_set = 0;
  arguments.output_file = NULL;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if(!cpu_supports_rdrand()) {
    printf("Error: Your CPU does not support RDRAND.\n");
    exit(1);
  }

  FILE* out = NULL;
  if(arguments.output_file) {
    out = fopen(arguments.output_file, "w+");
  }

  int blocks = 0;
  int blocks_left = arguments.block_count;
  int round_blocks = 0;
#define MAX_round_blocks 16
  uint64_t array[MAX_round_blocks];

  while(blocks_left > 0) {
    round_blocks = (blocks_left > MAX_round_blocks) ? MAX_round_blocks : blocks_left;

    blocks = rdrand_fill_array(array, round_blocks);

    if(blocks != round_blocks) {
      printf("rdrand round unsuccessful: %d != %d!\n", blocks, round_blocks);
    } else {
      if(out) {
        fwrite(array, sizeof(uint64_t), blocks, out);
      } else {
        printf("rdrand successful: %d\n", blocks);
        for(int i = 0; i <blocks; i++) {
          printf("%" PRIx64 "\n", array[i]);
        }
      }
      blocks_left -= round_blocks;
    }
  }

  if(out) {
    fclose(out);
  }

  return 0;
}
