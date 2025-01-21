#ifndef PIDFILE_H
#define PIDFILE_H

int read_pidfile(void);
void create_pidfile(void);
int unlink_pidfile(void);

#endif
