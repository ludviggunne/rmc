#ifndef MESSAGE_H
#define MESSAGE_H

#include <linux/limits.h>
#include <stdlib.h>

struct message {
  char *cwd;                    /* Clients working directory */
  char **env;                   /* Clients environment */
  char *cmd;                    /* Command to run */
};

/* These functions return error strings on failure,
   or NULL on success. */
const char *read_message(int fd, struct message *msg);
const char *write_message(int fd, struct message *msg);

void free_message(struct message *msg);

#endif
