#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client.h"
#include "escape.h"
#include "get_runtime_dir.h"
#include "message.h"

extern char **environ;

static struct message s_msg = { 0 };

static int s_sock = -1;

static void cleanup(void)
{
  if (s_sock >= 0)
    close(s_sock);
  s_msg.env = NULL;
  free_message(&s_msg);
}

void run_client(void)
{
  atexit(cleanup);

  char cwd_buf[PATH_MAX] = { 0 };
  if (getcwd(cwd_buf, sizeof(cwd_buf)) == NULL) {
    perror("getcwd");
    exit(EXIT_FAILURE);
  }

  s_msg.cwd = strdup(cwd_buf);
  s_msg.env = environ;
  s_msg.cmd = escape(g_command);

  struct sockaddr_un sockaddr = { 0 };
  snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path) - 1, "%s/rmc.sock",
           get_runtime_dir());
  sockaddr.sun_family = AF_UNIX;

  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (const struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    if (errno == ENOENT)
      /* Socket doesn't exist */
      fprintf(stderr, "error: no server running\n");
    else
      perror(sockaddr.sun_path);
    exit(EXIT_FAILURE);
  }

  if (write_message(sock, &s_msg) < 0) {
    perror("error: unable to write message");
    exit(EXIT_FAILURE);
  }

  exit(0);
}
