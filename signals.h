#ifndef SIGNALS_H
#define SIGNALS_H

#include <sys/signalfd.h>

int get_signalfd(void);
void unblock_signals(void);

#endif
