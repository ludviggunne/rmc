#ifndef XATEXIT_H
#define XATEXIT_H

void xatexit(void (*fn)(void));

void xatexit_enable(void);
void xatexit_disable(void);

#endif
