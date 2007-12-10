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
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include "internal.h"

int main_tty(int argc,char *argv[])
{
  printf("%s\n",ttyname(fileno(stdin)));
  return 0;
}

shell_cmd_t Shell_TTY_Command = {
  "tty",                                      /* name */
  "show ttyname",                             /* usage */
  "misc",                                     /* topic */
  main_tty  ,                                 /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
