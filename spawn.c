#include <stdio.h>
#include <string.h>

#include "signals.h"
#include "spawn.h"
#include "xatexit.h"

static char s_default_shell[] = "/bin/sh";
static char *s_shell = NULL;

static char *get_shell(void)
{
  if (s_shell == NULL) {
    s_shell = getenv("SHELL");
    if (s_shell == NULL)
      s_shell = s_default_shell;
  }
  return s_shell;
}

static char *get_shell_from_env(char **env)
{
  const char *key = "SHELL=";
  for (; *env; ++env) {
    if (strncmp(key, *env, strlen(key)) == 0)
      return *env + strlen(key);
  }
  return NULL;
}

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

    /* Check for SHELL in client environment,
     * otherwise check server environment,
     * otherwise use default /bin/sh */
    char *shell = get_shell_from_env(msg->env);
    if (shell == NULL)
      shell = get_shell();

    char *argv[] = { shell, "-c", msg->cmd, NULL };
    if (execve(argv[0], argv, msg->env) < 0) {
      perror("execve");
      exit(EXIT_FAILURE);
    }
  }

  return pid;
}
