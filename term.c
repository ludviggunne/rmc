#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "term.h"

static struct termios s_new, s_old;

static void cleanup(void)
{
  (void)tcsetattr(STDOUT_FILENO, TCSANOW, &s_old);
}

void disable_echoing(void)
{
  if (tcgetattr(STDOUT_FILENO, &s_old) < 0)
    /* Fail silently, it's not that important */
    return;
  s_new = s_old;
  /* We only want to disable echoing
   * for special characters, i.e C-c etc. */
  s_new.c_lflag &= ~ECHOCTL;
  if (tcsetattr(STDOUT_FILENO, TCSANOW, &s_new) < 0)
    return;
  atexit(cleanup);
}
