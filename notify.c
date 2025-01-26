#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <ctype.h>
#include <libnotify/notify.h>

#include "notify.h"
#include "opts.h"

static char *s_appname = NULL;
static char *s_body = NULL;
static char *s_last_command = NULL;

static void cleanup(void)
{
  free(s_appname);
  free(s_body);
  free(s_last_command);
  notify_uninit();
}

static void sprintf_realloc(char **buf, const char *format, ...)
{
  size_t len;
  va_list l1, l2;

  va_start(l1, format);
  va_copy(l2, l1);

  len = vsnprintf(NULL, 0, format, l1);
  va_end(l1);

  *buf = realloc(*buf, len + 1);

  vsprintf(*buf, format, l2);
  va_end(l2);
}

static void init(void)
{
  static int is_init = 0;
  if (is_init)
    return;
  sprintf_realloc(&s_appname, "rmc (%s)", g_name);
  notify_init(s_appname);
  is_init = 1;
  atexit(cleanup);
}

static void notify_send(int timeout)
{
  NotifyNotification *not;
  GError *error = NULL;
  init();
  not = notify_notification_new(s_appname, s_body, NULL);
  notify_notification_set_timeout(not, timeout);
  if (!notify_notification_show(not, &error)) {
    fprintf(stderr, "error: failed to send notification: %s\n", error->message);
  }
  g_object_unref(G_OBJECT(not));
}

void notify_start(struct message *msg)
{
  sprintf_realloc(&s_body, "Running command %s", msg->cmd);
  notify_send(NOTIFY_EXPIRES_DEFAULT);
  s_last_command = strdup(msg->cmd);
}

void notify_end(int status)
{
  int timeout = NOTIFY_EXPIRES_DEFAULT;
  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
    if (status == 0) {
      sprintf_realloc(&s_body, "Command finished: %s", s_last_command);
    } else {
      sprintf_realloc(&s_body, "Command failed (exit code %d): %s", status,
                      s_last_command);
      timeout = NOTIFY_EXPIRES_NEVER;
    }
  } else if (WIFSIGNALED(status)) {
    status = WTERMSIG(status);
    char buf[64];
    strncpy(buf, strsignal(status), sizeof(buf) - 1);
    buf[0] = tolower(buf[0]);
    sprintf_realloc(&s_body, "Command %s: %s", buf, s_last_command);
  } else {
    sprintf_realloc(&s_body, "Command stopped/continued: %s", s_last_command);
  }

  notify_send(timeout);

  free(s_last_command);
  s_last_command = NULL;
}
