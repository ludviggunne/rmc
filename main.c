#include <signal.h>
#include <stdlib.h>

#include "opts.h"
#include "usage.h"
#include "server.h"
#include "client.h"
#include "pidfile.h"

int main(int argc, char **argv)
{

  (void)argc;
  parse_args(argv);

  if (g_help) {
    if (g_help > 1)
      long_usage(stdout);
    else
      usage(stdout);
    exit(0);
  }

  if (g_getpid) {
    int pid = read_pidfile();
    if (pid > 0) {
      printf("%d\n", pid);
      exit(0);
    } else {
      fprintf(stderr, "error: no server running\n");
      exit(EXIT_FAILURE);
    }
  }

  if (g_kill) {
    int pid = read_pidfile();
    if (pid > 0)
      kill(pid, SIGTERM);
    exit(0);
  }

  if (g_cancel) {
    int pid = read_pidfile();
    if (pid > 0) {
      /* The server will cancel the running command
       * on SIGUSR1. */
      kill(pid, SIGUSR1);
      exit(0);
    } else {
      fprintf(stderr, "error: no server running\n");
      exit(EXIT_FAILURE);
    }
  }

  if (g_command)
    run_client();
  else
    run_server();
}
