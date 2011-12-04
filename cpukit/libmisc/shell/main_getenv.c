/*
 * Get an environment vairable.
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

static int rtems_shell_main_getenv(int argc, char *argv[])
{
  char* string;

  if (argc != 2)
  {
    printf ("error: only argument is the variable name\n");
    return 1;
  }

  string = getenv (argv[1]);

  if (!string)
  {
    printf ("error: %s not found\n", argv[1]);
    return 1;
  }

  printf ("%s\n", string);

  return 0;
}

rtems_shell_cmd_t rtems_shell_GETENV_Command = {
  "getenv",                      /* name */
  "getenv [var]",                /* usage */
  "misc",                        /* topic */
  rtems_shell_main_getenv,       /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
