/*
 * Set an environment vairable.
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

static int rtems_shell_main_setenv(int argc, char *argv[])
{
  char* env = NULL;
  char* string = NULL;
  int   len = 0;
  int   arg;
  char* p;

  if (argc <= 2) {
    printf("error: no variable or string\n");
    return 1;
  }

  env = argv[1];

  for (arg = 2; arg < argc; arg++)
    len += strlen(argv[arg]);

  len += argc - 2 - 1;

  string = malloc(len + 1);

  if (!string) {
    printf("error: no memory\n");
    return 1;
  }

  for (arg = 2, p = string; arg < argc; arg++) {
    strcpy(p, argv[arg]);
    p += strlen(argv[arg]);
    if (arg < (argc - 1)) {
      *p = ' ';
      p++;
    }
  }

  if (setenv(env, string, 1) < 0) {
    printf( "error: %s\n", strerror(errno) );
    free( string );
    return 1;
  }

  free( string );
  return 0;
}

rtems_shell_cmd_t rtems_shell_SETENV_Command = {
  "setenv",                      /* name */
  "setenv [var] [string]",       /* usage */
  "misc",                        /* topic */
  rtems_shell_main_setenv,       /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
