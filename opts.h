#ifndef OPTS_H
#define OPTS_H

#include <stdio.h>

extern const char *g_rmc;
extern const char *g_name;
extern int g_cancel;
extern int g_clear;
extern int g_help;
extern int g_reset;
extern int g_verbose;
extern int g_quiet;
extern int g_getpid;
extern int g_kill;
extern int g_list;
extern int g_daemon;
extern int g_notify;
extern char **g_command;

void parse_args(char **argv);

#endif
