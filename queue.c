#include <stdlib.h>
#include <assert.h>

#include "queue.h"

void queue_push(struct queue *q, struct msg *msg)
{
	struct qentry *e = calloc(1, sizeof *e);
	e->msg = msg;

	if (q->back)
	{
		e->next = q->back;
		q->back->prev = e;
		q->back = e;
	}
	else
	{
		q->back = e;
		q->front = e;
	}
}

struct msg *queue_pop(struct queue *q)
{
	assert(!queue_empty(q));
	struct msg *msg = q->front->msg;

	if (q->back == q->front)
	{
		free(q->back);
		q->back = NULL;
		q->front = NULL;
	}
	else
	{
		struct qentry *tmp = q->front;
		q->front = q->front->prev;
		q->front->next = NULL;
		free(tmp);
	}

	return msg;
}

int queue_empty(const struct queue *q)
{
	return !q->front;
}
