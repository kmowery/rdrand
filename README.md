rdrand
======

Small utilities for generating random numbers with RDRAND.


Usage
=====
Run ``rdrand`` to produce random numbers. For example, to generate
8 kibibytes (8192 bytes) of random data.

    ./rdrand --size=8k --output=8_kibibytes.txt

Without an output file, the random numbers will be printed in hexadecimal form.

It is also possible to set a core affinity, to restrict operation to a single
CPU core:

    ./rdrand -s 8k -o 8_kibibytes.txt -c 0

Continuity
=====
The largest RDRAND processor instruction returns data in 64-bit chunks. The
underlying generation process, however, uses 128-bit blocks. This utility was
created to piece together the underlying blocks.

To do so, it runs in rounds. Each round runs 16 consecutive 64 bit generation
attempts, and marks down successes. If any calls to RDRAND fail, the round
restarts.

