#ifndef NOTIFY_H
#define NOTIFY_H

#include "message.h"

void notify_start(struct message *msg);
void notify_end(int status);

#endif
