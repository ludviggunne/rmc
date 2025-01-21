#include <stdio.h>

#include "signals.h"
#include "spawn.h"
#include "xatexit.h"

pid_t spawn(struct message *msg)
{

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    xatexit_disable();
    unblock_signals();

    if (chdir(msg->cwd) < 0) {
      perror("chdir");
      exit(EXIT_FAILURE);
    }

    char *argv[] = { "/bin/sh", "-c", msg->cmd, NULL };
    if (execve(argv[0], argv, msg->env) < 0) {
      perror("execve");
      exit(EXIT_FAILURE);
    }
  }

  return pid;
}
