#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message.h"

struct spec {
  size_t cwdlen;
  size_t cmdlen;
  size_t nenv;
};

static int read_all(int fd, char *ptr, size_t len)
{
  ssize_t r;
  size_t off = 0;
  while (off < len) {
    r = read(fd, ptr + off, len - off);
    if (r <= 0)
      return -1;
    off += r;
  }
  return 0;
}

static int write_all(int fd, const char *ptr, size_t len)
{
  ssize_t w;
  size_t off = 0;
  while (off < len) {
    w = write(fd, ptr + off, len - off);
    if (w <= 0)
      return -1;
    off += w;
  }
  return 0;
}

int read_message(int fd, struct message *msg)
{

  struct spec spec;

  if (read_all(fd, (char *)&spec, sizeof(spec)) < 0)
    return -1;

  msg->cwd = malloc(spec.cwdlen);
  msg->cmd = malloc(spec.cmdlen);
  msg->env = calloc(spec.nenv + 1, sizeof(*msg->env));

  if (!(msg->cwd && msg->cmd && msg->env)) {
    free_message(msg);
    return -1;
  }
  if (read_all(fd, msg->cwd, spec.cwdlen) < 0) {
    free_message(msg);
    return -1;
  }
  if (read_all(fd, msg->cmd, spec.cmdlen) < 0) {
    free_message(msg);
    return -1;
  }

  for (size_t i = 0; i < spec.nenv; ++i) {
    size_t len;
    if (read_all(fd, (char *)&len, sizeof(len)) < 0) {
      free_message(msg);
      return -1;
    }
    msg->env[i] = malloc(len);
    if (read_all(fd, msg->env[i], len) < 0) {
      free_message(msg);
      return -1;
    }
  }

  msg->env[spec.nenv] = NULL;

  return 0;
}

int write_message(int fd, struct message *msg)
{

  struct spec spec;

  spec.cwdlen = strlen(msg->cwd) + 1;
  spec.cmdlen = strlen(msg->cmd) + 1;

  spec.nenv = 0;
  char **envptr = msg->env;
  for (; *envptr; ++envptr)
    ++spec.nenv;

  if (write_all(fd, (const char *)&spec, sizeof(spec)) < 0)
    return -1;
  if (write_all(fd, msg->cwd, spec.cwdlen) < 0)
    return -1;
  if (write_all(fd, msg->cmd, spec.cmdlen) < 0)
    return -1;

  envptr = msg->env;
  for (; *envptr; ++envptr) {
    size_t len = strlen(*envptr) + 1;
    if (write_all(fd, (char *)&len, sizeof(len)) < 0)
      return -1;
    if (write_all(fd, *envptr, len) < 0)
      return -1;
  }

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
