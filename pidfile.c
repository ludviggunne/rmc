#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>

#include "pidfile.h"
#include "get_runtime_dir.h"

void get_pidfile_path(char *path, size_t len)
{
  const char *dir = get_runtime_dir();
  snprintf(path, len, "%s/pid", dir);
}

int read_pidfile(void)
{
  FILE *f;
  char path[PATH_MAX];
  get_pidfile_path(path, sizeof(path));
  f = fopen(path, "r");
  if (f == NULL) {
    if (errno == ENOENT)
      return -1;
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  int pid;
  errno = 0;
  if (fscanf(f, "%d", &pid) != 1) {
    if (errno)
      perror("fscanf");
    else
      fprintf(stderr, "error: invalid PID file\n");
    exit(EXIT_FAILURE);
  }
  return pid;
}

void create_pidfile(void)
{
  FILE *f;
  char path[PATH_MAX];
  get_pidfile_path(path, sizeof(path));
  int pid = getpid();
  f = fopen(path, "w");
  if (f == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fprintf(f, "%d\n", pid);
  fclose(f);
}

int unlink_pidfile(void)
{
  char path[PATH_MAX];
  get_pidfile_path(path, sizeof(path));
  return unlink(path);
}
