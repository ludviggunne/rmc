#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msg.h"

static int read_all(FILE *f, void *ptr, size_t size)
{
	size_t offset = 0;
	while (offset < size)
	{
		ssize_t r = fread(ptr + offset, 1, size - offset, f);

		if (r < 0)
		{
			return -1;
		}

		offset += r;
	}

	return 0;
}

static int write_all(FILE *f, const void *ptr, size_t size)
{
	size_t offset = 0;
	while (offset < size)
	{
		ssize_t r = fwrite(ptr + offset, 1, size - offset, f);

		if (r < 0)
		{
			return -1;
		}

		offset += r;
	}

	return 0;
}

static char *read_str(FILE *f)
{
	size_t size;

	if (read_all(f, &size, sizeof size) < 0)
	{
		return NULL;
	}

	char *str = calloc(1, size + 1);

	if (read_all(f, str, size) < 0)
	{
		free(str);
		return NULL;
	}

	return str;
}

static int write_str(FILE *f, const char *str)
{
	size_t size = strlen(str);

	if (write_all(f, &size, sizeof size) < 0)
	{
		return -1;
	}

	if (write_all(f, str, size) < 0)
	{
		return -1;
	}

	return 0;
}

int msg_read(int fd, struct msg *msg)
{
	FILE *f = fdopen(fd, "r");

	msg->type = fgetc(f);

	switch (msg->type)
	{
	case MSG_COMMAND:
		break;

	case MSG_INTERRUPT:
	case MSG_KILL:
		return 0;

	default:
		goto fail;
	}

	if ((msg->cmd = read_str(f)) == NULL)
	{
		goto fail;
	}

	if ((msg->cwd = read_str(f)) == NULL)
	{
		goto fail;
	}

	size_t nenv;
	if (read_all(f, &nenv, sizeof nenv) < 0)
	{
		goto fail;
	}

	msg->env = calloc(1, sizeof(*msg->env) * (nenv + 1));

	for (size_t i = 0; i < nenv; i++)
	{
		if ((msg->env[i] = read_str(f)) == NULL)
		{
			goto fail;
		}
	}

	return 0;

fail:
	msg_free(msg);
	return -1;
}

int msg_write(int fd, const struct msg *msg)
{
	FILE *f = fdopen(fd, "w");

	if (fputc(msg->type, f) == EOF)
	{
		return -1;
	}

	if (msg->type == MSG_INTERRUPT || msg->type == MSG_KILL)
	{
		return 0;
	}

	if (write_str(f, msg->cmd) < 0)
	{
		return -1;
	}

	if (write_str(f, msg->cwd) < 0)
	{
		return -1;
	}

	size_t nenv = 0;
	for (char **ptr = msg->env; *ptr; ptr++)
	{
		nenv++;
	}

	if (write_all(f, &nenv, sizeof nenv) < 0)
	{
		return -1;
	}

	for (size_t i = 0; i < nenv; i++)
	{
		if (write_str(f, msg->env[i]) < 0)
		{
			return -1;
		}
	}

	return 0;
}

void msg_free(struct msg *msg)
{
	char **var = msg->env;
	while (var && *var)
	{
		free(*var);
		var++;
	}

	free(msg->env);
	free(msg->cmd);
	free(msg->cwd);
}
