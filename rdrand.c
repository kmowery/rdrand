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

int main(int argc, char** argv) {

  uint64_t x;
  unsigned char success;

  if(!cpu_supports_rdrand()) {
    printf("Error: Your CPU does not support RDRAND.\n");
    exit(1);
  }

  asm volatile("rdrand %0 ; setc %1"
    : "=r" (x), "=qm" (success));

  if(success) {
    printf("rdrand successful: %" PRIx64 "\n", x);
  } else {
    printf("rdrand failed: %" PRIx64 "\n", x);
  }

  return 0;
}
