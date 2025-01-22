#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>

static int s_fd = -1;

int get_signalfd(void)
{
  sigset_t mask;

  if (s_fd < 0) {
    (void)sigemptyset(&mask);
    (void)sigaddset(&mask, SIGINT);
    (void)sigaddset(&mask, SIGTERM);
    (void)sigaddset(&mask, SIGCHLD);
    (void)sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }
    s_fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (s_fd < 0) {
      perror("signalfd");
      exit(EXIT_FAILURE);
    }
  }
  return s_fd;
}

void unblock_signals(void)
{
  sigset_t mask;
  (void)sigemptyset(&mask);
  (void)sigaddset(&mask, SIGINT);
  (void)sigaddset(&mask, SIGTERM);
  (void)sigaddset(&mask, SIGCHLD);
  (void)sigaddset(&mask, SIGUSR1);
  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }
}
