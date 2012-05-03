/*
 *  LOGOFF Shell Command Implmentation
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

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_logoff(
  int   argc __attribute__((unused)),
  char *argv[] __attribute__((unused))
)
{
  printf("logoff from the system...");

  rtems_current_shell_env->exit_shell = true;

  return 0;
}

rtems_shell_cmd_t rtems_shell_LOGOFF_Command = {
  "logoff",                                  /* name */
  "logoff from the system",                  /* usage */
  "misc",                                    /* topic */
  rtems_shell_main_logoff,                   /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
