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
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

/*-----------------------------------------------------------*/
int main_logoff(int argc,char *argv[])
{
  printf("logoff from the system...");

  current_shell_env->exit_shell=TRUE;

  return 0;
}

shell_cmd_t Shell_LOGOFF_Command = {
  "logoff",                                  /* name */
  "logoff from the system",                  /* usage */
  "misc",                                    /* topic */
  main_logoff,                               /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
