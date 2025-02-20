#include <signal.h>
#include <stdlib.h>

#include "opts.h"
#include "usage.h"
#include "server.h"
#include "client.h"
#include "pidfile.h"
#include "list_servers.h"

int main(int argc, char **argv)
{

  (void)argc;
  parse_args(argv);

  if (g_help) {
    if (g_help > 1)
      long_usage(stdout);
    else
      short_usage(stdout);
    exit(0);
  }

  if (g_getpid) {
    int pid = read_pidfile();
    if (pid > 0) {
      printf("%d\n", pid);
      exit(0);
    }
    exit(EXIT_FAILURE);
  }

  if (g_list) {
    if (list_servers() < 0)
      exit(EXIT_FAILURE);
    exit(0);
  }

  if (g_kill) {
    int pid = read_pidfile();
    if (pid > 0) {
      kill(pid, SIGTERM);
      exit(0);
    }
    fprintf(stderr, "error: no server '%s' running\n", g_name);
    exit(EXIT_FAILURE);
  }

  if (g_cancel) {
    int pid = read_pidfile();
    if (pid > 0) {
      /* The server will cancel the running command
       * on SIGUSR1. */
      kill(pid, SIGUSR1);
      exit(0);
    }
    fprintf(stderr, "error: no server '%s' running\n", g_name);
    exit(EXIT_FAILURE);
  }

  if (g_command)
    run_client();
  else
    run_server();
}
