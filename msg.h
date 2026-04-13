#ifndef MSG_H
#define MSG_H

enum msg_type {
	MSG_COMMAND,
	MSG_INTERRUPT,
	MSG_KILL,
};

struct msg {
	int type;
	char *cmd;
	char *cwd;
	char **env;
};

int msg_read(int fd, struct msg *msg);
int msg_write(int fd, const struct msg *msg);
void msg_free(struct msg *msg);

#endif
