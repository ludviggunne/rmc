#ifndef CFG_H
#define CFG_H

#include <sys/un.h>

struct cfg {
	char *cmd;
	char *name;
	int clear;
	int echo;
	int interrupt;
	int kill;

	const void *sa_ptr;
	size_t sa_len;
};

void init_sockaddr(struct cfg *cfg);

#endif
