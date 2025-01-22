#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "signals.h"

static int s_fd = -1;

static void cleanup(void)
{
  if (s_fd != -1)
    close(s_fd);
}

int get_signalfd(void)
{
  sigset_t mask;

  if (s_fd < 0) {
    sigemptyset(&mask);
    /* These are the signals we
     * want to read with signalfd */
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }
    s_fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (s_fd < 0) {
      perror("signalfd");
      exit(EXIT_FAILURE);
    }
    atexit(cleanup);
  }
  return s_fd;
}

void unblock_signals(void)
{
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGCHLD);
  sigaddset(&mask, SIGUSR1);
  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }
}
