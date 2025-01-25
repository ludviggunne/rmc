#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>

#include "pidfile.h"
#include "runtime_path.h"
#include "opts.h"

static void get_pidfile_path(char *path, size_t len)
{
  const char *dir = runtime_path();
  snprintf(path, len, "%s/%s.pid", dir, g_name);
}

int read_pidfile(void)
{
  FILE *f;
  char path[PATH_MAX];
  get_pidfile_path(path, sizeof(path));
  f = fopen(path, "r");
  if (f == NULL) {
    if (errno == ENOENT)
      /* No server running */
      return -1;
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  int pid;
  errno = 0;
  if (fscanf(f, "%d", &pid) != 1) {
    /* TODO: silent failure? */
    if (errno)
      perror("fscanf");
    else
      fprintf(stderr, "error: invalid PID file\n");
    fclose(f);
    exit(EXIT_FAILURE);
  }
  fclose(f);
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
  int ret = unlink(path);
  return errno == ENOENT ? 0 : ret;
}
