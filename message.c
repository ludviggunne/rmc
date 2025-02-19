#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "message.h"

static int msg_getline(char **lineptr, FILE *f)
{
  size_t len;
  /* We won't be reusing any memory */
  *lineptr = NULL;
  if (getdelim(lineptr, &len, 0, f) < 0) {
    return -1;
  }
  return 0;
}

const char *read_message(int fd, struct message *msg)
{
  FILE *f = fdopen(fd, "r");
  if (f == NULL)
    return strerror(errno);

  memset(msg, 0, sizeof(*msg));

  if (msg_getline(&msg->cwd, f) < 0) {
    free_message(msg);
    return strerror(errno);
  }

  if (msg_getline(&msg->cmd, f) < 0) {
    free_message(msg);
    return strerror(errno);
  }

  char *nenv_buf = NULL;
  size_t nenv;

  if (msg_getline(&nenv_buf, f) < 0) {
    free(nenv_buf);
    free_message(msg);
    return strerror(errno);
  }

  int res = sscanf(nenv_buf, "%zu", &nenv);
  if (res != 1) {
    free(nenv_buf);
    free_message(msg);
    if (res == EOF)
      return "invalid nenv";
    return strerror(errno);
  }
  free(nenv_buf);

  msg->env = calloc(nenv, sizeof(*msg->env) + 1);
  for (size_t i = 0; i < nenv; ++i) {
    if (msg_getline(&msg->env[i], f) < 0) {
      free_message(msg);
      return strerror(errno);
    }
  }

  msg->env[nenv] = NULL;

  return 0;
}

const char *write_message(int fd, struct message *msg)
{
  FILE *f = fdopen(fd, "w");
  if (f == NULL)
    return strerror(errno);

  size_t nenv = 0;
  for (char **envptr = msg->env; *envptr; ++envptr)
    nenv++;

  if (fprintf(f, "%s%c", msg->cwd, 0) < 0)
    return "IO error";

  if (fprintf(f, "%s%c", msg->cmd, 0) < 0)
    return "IO error";

  if (fprintf(f, "%zu%c", nenv, 0) < 0)
    return "IO error";

  for (char **envptr = msg->env; *envptr; ++envptr) {
    if (fprintf(f, "%s%c", *envptr, 0) < 0)
      return "IO error";
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
