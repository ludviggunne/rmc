#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "escape.h"

struct buffer {
  char *ptr;
  size_t len;
  size_t cap;
};

void buf_putc(struct buffer *buf, char c)
{
  if (buf->ptr == NULL) {
    buf->cap = 32;
    buf->ptr = malloc(buf->cap);
    buf->len = 0;
    if (buf->ptr == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
  }
  if (buf->len == buf->cap) {
    buf->cap *= 2;
    buf->ptr = realloc(buf->ptr, buf->cap);
  }
  buf->ptr[buf->len++] = c;
}

char *escape(char **argv)
{
  struct buffer b = { 0 };
  for (; *argv; ++argv) {
    char *arg = *argv;
    for (; *arg; ++arg) {
      char c = *arg;
      switch (c) {
      case ' ':
      case '\\':
      case '\'':
      case '\"':
        buf_putc(&b, '\\');
        break;
      default:
        break;
      }
      buf_putc(&b, c);
    }
    if (*(argv + 1))
      buf_putc(&b, ' ');
  }
  return b.ptr;
}
