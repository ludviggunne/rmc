#include "usage.h"
#include "opts.h"

#include <assert.h>

void usage(FILE *f)
{
  assert(g_name);
  fprintf(f, "Usage: %s [OPTIONS...] [COMMAND]\n", g_name);
}

void long_usage(FILE *f)
{
  usage(f);
  fprintf(f, "Options:\n"
          "    -v, --verbose   Print working directory and command string before each run.\n"
          "    -c, --clear     Clear the terminal in between each run.\n"
          "    -h, --help      Show this help message.\n"
          "    -r, --reset     Interrupt the running command if a new one is received.\n"
          "    -q, --quiet     Don't print exit status. Cancels out --verbose.\n"
          "    -C, --cancel    Cancel running command remotely.\n"
          "    -p, --pid       Get process ID of server.\n");
}
