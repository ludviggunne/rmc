#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "message.h"

static int msg_getline(char **lineptr, FILE *f)
{
  size_t len;
  /* We won't be reusing any memory */
  *lineptr = NULL;
  if (getline(lineptr, &len, f) < 0) {
    return -1;
  }
  /* Get rid of newline */
  (*lineptr)[strlen(*lineptr) - 1] = 0;
  return 0;
}

int read_message(int fd, struct message *msg)
{
  FILE *f = fdopen(fd, "r");
  if (f == NULL)
    return -1;

  memset(msg, 0, sizeof(*msg));

  if (msg_getline(&msg->cwd, f) < 0) {
    free_message(msg);
    return -1;
  }

  if (msg_getline(&msg->cmd, f) < 0) {
    free_message(msg);
    return -1;
  }

  char *nenv_buf = NULL;
  size_t nenv;

  if (msg_getline(&nenv_buf, f) < 0) {
    free(nenv_buf);
    free_message(msg);
    return -1;
  }

  if (sscanf(nenv_buf, "%zu", &nenv) != 1) {
    free(nenv_buf);
    free_message(msg);
    return -1;
  }
  free(nenv_buf);

  msg->env = calloc(nenv, sizeof(*msg->env) + 1);
  for (size_t i = 0; i < nenv; ++i) {
    if (msg_getline(&msg->env[i], f) < 0) {
      free_message(msg);
      return -1;
    }
  }

  msg->env[nenv] = NULL;

  return 0;
}

int write_message(int fd, struct message *msg)
{
  FILE *f = fdopen(fd, "w");
  if (f == NULL)
    return -1;

  size_t nenv = 0;
  for (char **envptr = msg->env; *envptr; ++envptr)
    nenv++;

  if (fprintf(f, "%s\n", msg->cwd) < 0)
    return -1;

  if (fprintf(f, "%s\n", msg->cmd) < 0)
    return -1;

  if (fprintf(f, "%zu\n", nenv) < 0)
    return -1;

  for (char **envptr = msg->env; *envptr; ++envptr) {
    if (fprintf(f, "%s\n", *envptr) < 0)
      return -1;
  }

  fflush(f);

  return 0;
}

void free_message(struct message *msg)
{
  if (msg->cwd)
    free(msg->cwd);
  if (msg->cmd)
    free(msg->cmd);
  if (msg->env) {
    for (char **envptr = msg->env; *envptr; ++envptr)
      free(*envptr);
    free(msg->env);
  }
}
