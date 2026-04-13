#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>

#include "msg.h"
#include "cfg.h"

extern char **environ;

int run_client(const struct cfg *cfg)
{
	char buf[PATH_MAX];

	struct msg msg;

	if (cfg->kill)
	{
		msg.type = MSG_KILL;
	}
	else if (cfg->interrupt)
	{
		msg.type = MSG_INTERRUPT;
	}
	else
	{
		msg.type = MSG_COMMAND;
		msg.cmd = cfg->cmd;
		msg.cwd = getcwd(buf, sizeof buf);
		msg.env = environ;
	}

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket()");
		return EXIT_FAILURE;
	}

	if (connect(sock, cfg->sa_ptr, cfg->sa_len) < 0)
	{
		if (errno == ECONNREFUSED)
		{
			fprintf(stderr, "error: no such server: %s\n", cfg->name);
		}
		else
		{
			perror("connect()");
		}

		return EXIT_FAILURE;
	}

	if (msg_write(sock, &msg) < 0)
	{
		fprintf(stderr, "error: failed to write message\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
