#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <errno.h>

#include "get_runtime_dir.h"

static char s_runtime_dir[PATH_MAX] = { 0 };

const char *get_runtime_dir(void)
{
  if (s_runtime_dir[0] == 0) {
    char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (xdg_runtime_dir == NULL) {
      /* TODO: fallback? */
      fprintf(stderr, "error: XDG_RUNTIME_DIR is not defined");
      exit(EXIT_FAILURE);
    }
    snprintf(s_runtime_dir, sizeof(s_runtime_dir), "%s/rmc", xdg_runtime_dir);
    /* Make sure the directory exists */
    if (mkdir(s_runtime_dir, 0777) < 0 && errno != EEXIST) {
      perror("mkdir");
      exit(EXIT_FAILURE);
    }
  }
  return s_runtime_dir;
}
