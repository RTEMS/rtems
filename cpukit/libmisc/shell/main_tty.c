/*
 *  TTY Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_tty(
  int   argc __attribute__((unused)),
  char *argv[] __attribute__((unused))
)
{
  printf("%s\n", ttyname(fileno(stdin)));
  return 0;
}

rtems_shell_cmd_t rtems_shell_TTY_Command = {
  "tty",                                      /* name */
  "show ttyname",                             /* usage */
  "misc",                                     /* topic */
  rtems_shell_main_tty,                       /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
