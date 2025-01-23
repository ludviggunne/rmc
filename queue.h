#ifndef QUEUE_H
#define QUEUE_H

#include "message.h"

void enqueue_message(const struct message *msg);
struct message *peek_message(void);
void pop_message(void);
void pop_all_but_one_message(void);

#endif
