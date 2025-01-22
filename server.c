#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "get_runtime_dir.h"
#include "message.h"
#include "server.h"
#include "signals.h"
#include "spawn.h"
#include "xatexit.h"
#include "term.h"
#include "queue.h"
#include "opts.h"
#include "pidfile.h"

static struct sockaddr_un s_sockaddr = { 0 };

static int s_sock = -1;
static int s_pid = NO_PID;

static void cleanup(void)
{
  if (s_sock >= 0 && close(s_sock) < 0)
    perror("close");
  if (unlink(s_sockaddr.sun_path) < 0)
    perror("unlink");
  if (unlink_pidfile() < 0)
    perror("unlink");
}

static void handle_connection(void);
static void handle_signal(void);
static void print_status(int status);
static void clear(void);

void run_server(void)
{
  snprintf(s_sockaddr.sun_path, sizeof(s_sockaddr.sun_path) - 1, "%s/rmc.sock",
           get_runtime_dir());
  s_sockaddr.sun_family = AF_UNIX;

  if (access(s_sockaddr.sun_path, F_OK) == 0) {
    fprintf(stderr, "error: server is already running\n");
    exit(EXIT_FAILURE);
  }

  create_pidfile();

  /* Runtime files are created,
   * we can enable exit handler */
  xatexit_enable();
  xatexit(cleanup);

  disable_echoing();

  int sigfd = get_signalfd();

  s_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (s_sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (bind(s_sock, (const struct sockaddr *)&s_sockaddr, sizeof(s_sockaddr)) <
      0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(s_sock, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  enum { SIGFD, SOCKFD };
  struct pollfd pfd[2] = { 0 };
  pfd[SIGFD].events = POLLIN;
  pfd[SIGFD].fd = sigfd;
  pfd[SOCKFD].events = POLLIN;
  pfd[SOCKFD].fd = s_sock;

  if (g_clear)
    clear();

  for (;;) {

    if (poll(pfd, 2, -1) < 0) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    if (pfd[SOCKFD].revents & POLLIN) {
      pfd[SOCKFD].revents = 0;
      handle_connection();
    }

    if (pfd[SIGFD].revents & POLLIN) {
      pfd[SIGFD].revents = 0;
      handle_signal();
    }

    /* Are there queued messages? */
    struct message *msg = peek_message();
    if (msg) {
      if (s_pid == NO_PID) {
        /* No command running, run command
         * at beginning of queue. */
        if (g_clear)
          clear();
        if (g_verbose) {
          printf("%s\n%s\n", msg->cwd, msg->cmd);
        }
        s_pid = spawn(msg);
        pop_message();
      } else if (g_reset) {
        /* Interrupt running command,
         * and cancel all but the latest
         * received command. */
        pop_all_but_one_message();
        kill(s_pid, SIGINT);
      }
    }
  }
}

static void handle_connection(void)
{
  struct message msg;
  int conn = accept(s_sock, NULL, NULL);
  if (conn < 0) {
    perror("accept");
    return;
  }
  if (read_message(conn, &msg) < 0) {
    fprintf(stderr, "error: unable to read message: %s\n", strerror(errno));
  } else {
    enqueue_message(&msg);
  }
  close(conn);
}

static void handle_signal(void)
{
  struct signalfd_siginfo info;
  int sigfd = get_signalfd();
  if (read(sigfd, &info, sizeof(info)) < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  switch (info.ssi_signo) {
  case SIGTERM:
  case SIGINT:
    {
      /* Exit if there is no command
       * running, otherwise cancel
       * the command. */
      if (s_pid == NO_PID)
        exit(0);
      else
        kill(s_pid, info.ssi_signo);
      break;
    }
  case SIGCHLD:
    {
      /* Command finished */
      int status;
      if (wait(&status) < 0) {
        perror("wait");
        break;
      }
      s_pid = NO_PID;
      if (!g_quiet)
        print_status(status);
      break;
    }
  case SIGUSR1:
    {
      if (s_pid != NO_PID)
        /* Cancel running command */
        kill(s_pid, SIGTERM);
      break;
    }
  default:
    assert(0);
  }
}

static void print_status(int status)
{
  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
    if (status == 0)
      printf("\x1b[32m");
    else
      printf("\x1b[31m");
    printf("Exit status: %d\x1b[0m\n", status);
  } else if (WIFSIGNALED(status)) {
    const char *sigstr = strsignal(WTERMSIG(status));
    printf("\n\x1b[33m%s\x1b[0m\n", sigstr);
  }
}

static void clear(void)
{
  printf("\x1b[2J\x1b[H");
  fflush(stdout);
}
