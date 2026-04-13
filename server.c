#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include "msg.h"
#include "queue.h"
#include "cfg.h"

struct server {
	int sock;
	int sig_fd;
	long pid;
	struct queue queue;
	const struct cfg *cfg;
};

static void clear(void)
{
	printf("\e[2J\e[H");
	fflush(stdout);
}

static int init(struct server *srv, const struct cfg *cfg)
{
	srv->cfg = cfg;
	srv->pid = -1;
	srv->queue = (struct queue) {0};
	srv->sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (srv->sock < 0)
	{
		perror("socket()");
		return -1;
	}

	if (bind(srv->sock, cfg->sa_ptr, cfg->sa_len) < 0)
	{
		if (errno == EADDRINUSE)
		{
			fprintf(stderr,
			        "error: server '%s' already exists\n",
			        cfg->name);
		}
		else
		{
			perror("bind()");
		}
		return -1;
	}

	if (listen(srv->sock, 10) < 0)
	{
		perror("listen()");
		return -1;
	}

	sigset_t msk;
	sigaddset(&msk, SIGINT);
	sigaddset(&msk, SIGCHLD);

	if (sigprocmask(SIG_BLOCK, &msk, NULL) < 0)
	{
		perror("sigprocmask()");
		return -1;
	}

	if ((srv->sig_fd = signalfd(-1, &msk, SFD_CLOEXEC)) < 0)
	{
		perror("signalfd()");
		return -1;
	}

	if (cfg->clear)
	{
		clear();
	}

	return 0;
}

static int handle_msg(struct server *srv)
{
	int conn = accept(srv->sock, NULL, NULL);
	if (conn < 0)
	{
		perror("accept()");
		return -1;
	}

	struct msg *msg = calloc(1, sizeof *msg);
	int res = msg_read(conn, msg);
	close(conn);

	if (res < 0)
	{
		free(msg);
		fprintf(stderr, "error: failed to read message\n");
		return 0;
	}

	switch (msg->type)
	{
	case MSG_INTERRUPT:
		if (srv->pid > 0)
		{
			kill(srv->pid, SIGINT);
		}
		msg_free(msg);
		free(msg);
		break;

	case MSG_KILL:
		exit(EXIT_SUCCESS);
		break;

	case MSG_COMMAND:
		queue_push(&srv->queue, msg);
		break;
	}

	return 0;
}

static int handle_sigint(struct server *srv)
{
	if (srv->pid < 0)
	{
		exit(EXIT_SUCCESS);
	}
	else
	{
		kill(srv->pid, SIGINT);
	}

	return 0;
}

static int handle_sigchld(struct server *srv)
{
	int status;
	wait(&status);
	srv->pid = -1;
	return 0;
}

static int handle_signal(struct server *srv)
{
	struct signalfd_siginfo info = {0};
	size_t offset = 0;

	while (offset < sizeof info)
	{
		ssize_t r = read(srv->sig_fd, &info + offset, sizeof info - offset);

		if (r < 0)
		{
			return -1;
		}

		offset += r;
	}

	switch (info.ssi_signo)
	{
	case SIGINT:
		return handle_sigint(srv);
	case SIGCHLD:
		return handle_sigchld(srv);
	default:
		assert(0);
	}
}

static int launch_cmd(struct server *srv, const struct msg *msg)
{
	if (srv->cfg->clear)
	{
		clear();
	}

	if (srv->cfg->echo)
	{
		printf("%s\n", msg->cmd);
	}

	srv->pid = fork();

	if (srv->pid < 0)
	{
		perror("fork()");
		return -1;
	}

	if (srv->pid > 0)
	{
		return 0;
	}

	if (chdir(msg->cwd) < 0)
	{
		perror("chdir()");
		_Exit(EXIT_FAILURE);
	}

	sigset_t msk;
	sigaddset(&msk, SIGINT);
	sigaddset(&msk, SIGCHLD);

	if (sigprocmask(SIG_UNBLOCK, &msk, NULL) < 0)
	{
		perror("sigprocmask()");
		return -1;
	}

	char *argv[] = { "/bin/sh", "-c", msg->cmd, NULL };

	if (execve(argv[0], argv, msg->env) < 0)
	{
		perror("execve()");
		_Exit(EXIT_FAILURE);
	}

	__builtin_unreachable();
}

static int loop(struct server *srv)
{
	struct pollfd pfds[2];

	pfds[0].fd = srv->sock;
	pfds[0].events = POLLIN;

	pfds[1].fd = srv->sig_fd;
	pfds[1].events = POLLIN;

	if (poll(pfds, 2, -1) < 0)
	{
		perror("poll()");
		return -1;
	}

	if ((pfds[0].revents & POLLIN) && handle_msg(srv) < 0)
	{
		return -1;
	}

	if ((pfds[1].revents & POLLIN) && handle_signal(srv) < 0)
	{
		return -1;
	}

	if (srv->pid == -1 && !queue_empty(&srv->queue))
	{
		struct msg *msg = queue_pop(&srv->queue);

		if (launch_cmd(srv, msg) < 0)
		{
			return -1;
		}

		msg_free(msg);
		free(msg);
	}

	return 0;
}

int run_server(const struct cfg *cfg)
{
	struct server srv = {0};

	if (init(&srv, cfg) < 0)
	{
		return EXIT_FAILURE;
	}

	for (;;)
	{
		if (loop(&srv) < 0)
		{
			return EXIT_FAILURE;
		}
	}
}
