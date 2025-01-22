#include <stdlib.h>

#include "xatexit.h"

struct handler {
  void (*fn)(void);
  struct handler *next;
};

static int s_is_reg = 0;
static int s_enabled = 1;
static struct handler *s_handlers = NULL;

static void run_handlers(void)
{
  struct handler *tmp = NULL;
  if (!s_enabled)
    return;
  while (s_handlers) {
    s_handlers->fn();
    tmp = s_handlers->next;
    free(s_handlers);
    s_handlers = tmp;
  }
}

void xatexit(void (*fn)(void))
{
  /* We manually register exit handlers so
   * that we can disable them in child
   * processes. */
  struct handler *handler = NULL;
  if (!s_is_reg)
    atexit(run_handlers);
  s_is_reg = 1;
  handler = malloc(sizeof(*handler));
  handler->fn = fn;
  handler->next = s_handlers;
  s_handlers = handler;
}

void xatexit_enable(void)
{
  s_enabled = 1;
}

void xatexit_disable(void)
{
  s_enabled = 0;
}
