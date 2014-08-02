#ifndef arguments_h
#define arguments_h

#include <string.h>
#include <argp.h>

const char *argp_program_version =
"rdrand 0.1";
const char *argp_program_bug_address =
"<kmowery@cs.ucsd.edu>";

static char doc[] =
"rdrand -- generate blocks of random numbers using Intel's RDRAND technology";

static char args_doc[] = "";

static struct argp_option options[] = {
  {"blocks",     'b', "BLOCKS",      0,  "Generate N 64-bit blocks of randomness" },
  {"size",       's', "BYTES",       0,  "Generate N bytes of randomness. Must be an integer multiple of 8 bytes (64 bits). Accepts k, m, g, and t suffixes (2^x)." },
  {"core",       'c', "CORE",        0,  "Set processor core affinity to this core" },
  {"output",     'o', "FILE", 0,
    "Output to FILE. Can specify multiple times; each file will receive B blocks of randomness." },
  { 0 }
};

#define TOO_MANY_SIZES "Please specify only one of: --blocks/-b, --size/-s.\n"

struct arguments
{
  uint64_t block_count; // number of 64-bit blocks to output
  int block_count_set;
  uint32_t core;
  char *output_file;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  uint64_t multiplier = 1;
  int len;

  switch (key)
  {
    case 'b':
      if(arguments->block_count_set) {
        argp_error(state, TOO_MANY_SIZES);
      }
      arguments->block_count = atoi(arg);
      arguments->block_count_set = 1;
      break;
    case 'c':
      arguments->core = atoi(arg);
      break;
    case 's':
      if(arguments->block_count_set) {
        argp_error(state, TOO_MANY_SIZES);
      }
      len = strlen(arg);
      // Check for 'k', 'm', 'g', and 't' suffixes.
      if(arg[len-1] == 'k' || arg[len-1] == 'K') {
        multiplier <<= 10;
        arg[len-1] = '\0';
      } else if(arg[len-1] == 'm' || arg[len-1] == 'M') {
        multiplier <<= 20;
        arg[len-1] = '\0';
      } else if(arg[len-1] == 'g' || arg[len-1] == 'G') {
        multiplier <<= 30;
        arg[len-1] = '\0';
      } else if(arg[len-1] == 't' || arg[len-1] == 'T') {
        multiplier <<= 40;
        arg[len-1] = '\0';
      }

      // Divide by 8 here to correct for each block being 64-bits long
      arguments->block_count = atoi(arg) * multiplier / 8;
      arguments->block_count_set = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;

    case ARGP_KEY_ARG:
      /* Too many arguments. */
      argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

#endif
