#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "list_servers.h"
#include "get_runtime_dir.h"

int list_servers(void)
{
  const char *path = get_runtime_dir();
  DIR *dir = opendir(path);
  if (dir == NULL) {
    if (errno != ENOENT)
      perror("opendir");
    return -1;
  }

  int ret = -1;
  const struct dirent *de;
  while ((de = readdir(dir)) != NULL) {
    const char *p = strrchr(de->d_name, '.');
    if (p == NULL || strcmp(p, ".pid") != 0)
      continue;
    ret = 0;
    printf("%.*s\n", (int)(p - de->d_name), de->d_name);
  }

  return ret;
}