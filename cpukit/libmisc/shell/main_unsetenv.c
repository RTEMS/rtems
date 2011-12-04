/*
 * Unset an environment vairable.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_unsetenv(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf ("error: only argument is the variable name\n");
    return 1;
  }

  if (unsetenv (argv[1]) < 0)
  {
    printf ("error: %s\n", strerror (errno));
    return 1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_UNSETENV_Command = {
  "unsetenv",                    /* name */
  "unsetenv [var]",              /* usage */
  "misc",                        /* topic */
  rtems_shell_main_unsetenv,     /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
