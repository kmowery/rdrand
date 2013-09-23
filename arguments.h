#ifndef arguments_h
#define arguments_h

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
  {"output",   'o', "FILE", 0,
    "Output to FILE. Can specify multiple times; each file will receive B blocks of randomness." },
  { 0 }
};

struct arguments
{
  int block_count; // number of 64-bit blocks to output
  char *output_file;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
  {
    case 'b':
      arguments->block_count = atoi(arg);
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
