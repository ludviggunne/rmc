#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
  struct message *msg;
  struct queue *next, *prev;
};

static struct queue *s_front = NULL;
static struct queue *s_back = NULL;

void enqueue_message(struct message *msg)
{
  /* We make a shallow copy of the message,
   * so the original message should not be freed! */
  struct message *copy = malloc(sizeof(struct message));
  memcpy(copy, msg, sizeof(struct message));

  struct queue *q = calloc(1, sizeof(struct queue));
  q->msg = copy;

  if (s_back) {
    s_back->prev = q;
    q->next = s_back;
    s_back = q;
  } else {
    s_back = s_front = q;
  }
}

struct message *peek_message(void)
{
  if (s_front)
    return s_front->msg;
  return NULL;
}

void pop_message(void)
{
  struct queue *q = s_front;
  if (!q)
    return;

  if (s_front->prev) {
    s_front = q->prev;
    s_front->next = NULL;
  } else {
    s_front = s_back = NULL;
  }

  free_message(q->msg);
  free(q);
}

void pop_all_but_one_message(void)
{
  while (s_front->prev)
    pop_message();
}
