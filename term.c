#include <termios.h>
#include <unistd.h>

#include "term.h"
#include "xatexit.h"

static struct termios s_new, s_old;

static void cleanup(void)
{
  (void)tcsetattr(STDOUT_FILENO, TCSANOW, &s_old);
}

void disable_echoing(void)
{
  if (tcgetattr(STDOUT_FILENO, &s_old) < 0)
    return;
  s_new = s_old;
  s_new.c_lflag &= ~ECHOCTL;
  if (tcsetattr(STDOUT_FILENO, TCSANOW, &s_new) < 0)
    return;
  xatexit(cleanup);
}
