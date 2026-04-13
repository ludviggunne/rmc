#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "cfg.h"

extern int run_server(const struct cfg *cfg);
extern int run_client(const struct cfg *cfg);

static const char *program;

static void usage(FILE *f)
{
	fprintf(stderr,
		"Usage: %s [OPTION...] [COMMAND]\n"
		"Options:\n"
		"\t-n NAME\tStart/select server NAME\n"
		"\t-c\tClear terminal between commands\n"
		"\t-e\tEcho commmands\n"
		"\t-i\tInterrupt current command\n"
		"\t-k\tKill server\n"
		"\t-l\tList servers\n"
		"\t-h\tShow this help message\n"
		, program);
}

static char *get_name(char *line)
{
	char *f, *ptr = line;
	const char *prefix = "@rmc.";

	while (f = strtok(ptr, " \t\n"))
	{
		if (strncmp(prefix, f, strlen(prefix)) == 0)
		{
			return f + strlen(prefix);
		}

		ptr = NULL;
	}

	return NULL;
}

static int list_servers(void)
{
	FILE *f = fopen("/proc/net/unix", "r");

	if (f == NULL)
	{
		perror("fopen()");
		return EXIT_FAILURE;
	}

	char **list = NULL;
	int count = 0;

	char *line = NULL;
	size_t size;

	while (getline(&line, &size, f) >= 0)
	{
		char *name = get_name(line);

		if (name == NULL)
		{
			continue;
		}

		count++;
		list = realloc(list, sizeof(*list) * count);
		list[count-1] = strdup(name);
	}

	free(line);

	for (int i = 0; i < count; i++)
	{
		int dupl = 0;
		for (int j = i + 1; j < count; j++)
		{
			if (strcmp(list[i], list[j]) == 0)
			{
				dupl = 1;
				break;
			}
		}

		if (dupl)
		{
			free(list[i]);
			continue;
		}

		printf("%s\n", list[i]);
		free(list[i]);
	}

	fclose(f);
	free(list);

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	program = argv[0];

	struct cfg cfg = {0};
	cfg.name = strdup("default");

	int c;
	while ((c = getopt(argc, argv, "chiken:l")) != -1)
	{
		switch (c)
		{
		case 'i':
			cfg.interrupt = 1;
			break;
		case 'k':
			cfg.kill = 1;
			break;
		case 'e':
			cfg.echo = 1;
			break;
		case 'c':
			cfg.clear = 1;
			break;
		case 'n':
			free(cfg.name);
			cfg.name = strdup(optarg);
			break;
		case 'h':
			usage(stdout);
			exit(EXIT_SUCCESS);
		case 'l':
			exit(list_servers());
		default:
			usage(stderr);
			exit(EXIT_FAILURE);
		}
	}

	char buf[4096] = {0};
	while (argv[optind])
	{
		cfg.cmd = buf + 1;
		strcat(buf, " ");
		strcat(buf, argv[optind]);
		optind++;
	}

	struct sockaddr_un sa;
	sa.sun_family = AF_UNIX;
	sa.sun_path[0] = 0;
	snprintf(&sa.sun_path[1], sizeof(sa.sun_path) - 1, "rmc.%s", cfg.name);

	cfg.sa_ptr = &sa;
	cfg.sa_len = sizeof sa.sun_family + strlen(&sa.sun_path[1]) + 1;

	if (cfg.cmd || cfg.interrupt || cfg.kill)
	{
		exit(run_client(&cfg));
	}
	else
	{
		exit(run_server(&cfg));
	}
}
