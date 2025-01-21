#ifndef SPAWN_H
#define SPAWN_H

#include "message.h"
#include <unistd.h>

#define NO_PID -1

pid_t spawn(struct message *msg);

#endif
