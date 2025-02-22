#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <errno.h>

#include "runtime_path.h"

static char s_runtime_path[PATH_MAX] = { 0 };

const char *runtime_path(void)
{
  if (s_runtime_path[0] == 0) {
    const char *xdg_runtime_path = getenv("XDG_RUNTIME_DIR");
    if (xdg_runtime_path == NULL) {
      uid_t uid = getuid();
      snprintf(s_runtime_path, sizeof(s_runtime_path), "/run/user/%d/rmc", uid);
    } else {
      snprintf(s_runtime_path, sizeof(s_runtime_path), "%s/rmc",
               xdg_runtime_path);
    }
    /* Make sure the directory exists */
    if (mkdir(s_runtime_path, 0777) < 0 && errno != EEXIST) {
      perror("mkdir");
      exit(EXIT_FAILURE);
    }
  }
  return s_runtime_path;
}
