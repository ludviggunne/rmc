#ifndef XATEXIT_H
#define XATEXIT_H

/* We manually register exit handlers so
 * that we can disable them in child
 * processes. */
void xatexit(void (*fn)(void));

void xatexit_enable(void);
void xatexit_disable(void);

#endif
