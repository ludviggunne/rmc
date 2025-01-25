#include "usage.h"
#include "opts.h"

#include <assert.h>

static void usage(FILE *f)
{
  assert(g_rmc);
  fprintf(f, "Usage: %s [OPTIONS...] [COMMAND]\n", g_rmc);
}

void short_usage(FILE *f)
{
  usage(f);
  fprintf(f, "Try 'rmc --help' for more information.\n");
}

void long_usage(FILE *f)
{
  usage(f);
  fprintf(f, "Options:\n"
          "    -v, --verbose     Print working directory and command string before each run.\n"
          "    -c, --clear       Clear the terminal in between each run.\n"
          "    -h, --help        Show this help message.\n"
          "    -r, --reset       Interrupt the running command if a new one is received.\n"
          "    -q, --quiet       Don't print exit status. Cancels out --verbose.\n"
          "    -p, --pid         Get process ID of server.\n"
          "    -C, --cancel      Cancel running command remotely (shorthand for 'kill -SIGUSR1 $(rmc --pid)').\n"
          "    -k, --kill        Kill the server (shorthand for 'kill -SIGTERM $(rmc --pid)').\n"
          "    -l, --list        List active servers.\n"
          "    -d, --daemon      Start in daemon mode.\n"
          "    -n, --name=<name> Send command to/start server with name <name>. The default name is 'default'.\n");
  fprintf(f, "Try 'man rmc' for more information.\n");
}
