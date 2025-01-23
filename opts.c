#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opts.h"
#include "usage.h"

const char *g_name = NULL;
int g_cancel = 0;
int g_clear = 0;
int g_help = 0;
int g_reset = 0;
int g_verbose = 0;
int g_quiet = 0;
int g_getpid = 0;
int g_kill = 0;
char **g_command = NULL;

void parse_args(char **argv)
{
  g_name = *argv;
  ++argv;

  for (; *argv; ++argv) {
    if (strncmp(*argv, "--", 2) == 0) {
      if (strcmp(*argv, "--cancel") == 0) {
        g_cancel = 1;
        continue;
      }
      if (strcmp(*argv, "--clear") == 0) {
        g_clear = 1;
        continue;
      }
      if (strcmp(*argv, "--help") == 0) {
        g_help = 2;
        continue;
      }
      if (strcmp(*argv, "--reset") == 0) {
        g_reset = 1;
        continue;
      }
      if (strcmp(*argv, "--verbose") == 0) {
        g_verbose++;
        continue;
      }
      if (strcmp(*argv, "--quiet") == 0) {
        g_quiet = 1;
        continue;
      }
      if (strcmp(*argv, "--pid") == 0) {
        g_getpid = 1;
        continue;
      }
      if (strcmp(*argv, "--kill") == 0) {
        g_kill = 1;
        continue;
      }
      short_usage(stderr);
      if (strlen(*argv) == 2)
        fprintf(stderr, "error: empty option\n");
      else
        fprintf(stderr, "invalid option '%s'\n", *argv);
      exit(EXIT_FAILURE);
    }

    if (*argv[0] == '-') {
      char *arg = *argv;
      ++arg;
      for (; *arg; ++arg) {
        switch (*arg) {
        case 'C':
          g_cancel = 1;
          continue;
        case 'c':
          g_clear = 1;
          continue;
        case 'h':
          g_help = g_help ? g_help : 1;
          continue;
        case 'r':
          g_reset = 1;
          continue;
        case 'v':
          g_verbose++;
          continue;
        case 'q':
          g_quiet = 1;
          continue;
        case 'p':
          g_getpid = 1;
          continue;
        case 'k':
          g_kill = 1;
          continue;
        default:
          short_usage(stderr);
          fprintf(stderr, "invalid option '%c'\n", *arg);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    g_command = argv;
    break;
  }

  if (g_quiet)
    g_verbose = 0;
}
