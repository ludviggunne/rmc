#ifndef QUEUE_H
#define QUEUE_H

#include "msg.h"

struct qentry {
	struct qentry *next;
	struct qentry *prev;
	struct msg *msg;
};

struct queue {
	struct qentry *front;
	struct qentry *back;
};

void queue_push(struct queue *q, struct msg *msg);
struct msg *queue_pop(struct queue *q);
int queue_empty(const struct queue *q);

#endif
