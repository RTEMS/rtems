/*
 *  DATE Shell Command Implmentation
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
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_date(int argc,char *argv[])
{
  time_t t;

  time(&t);
  fprintf(stdout,"%s", ctime(&t));
  return 0;
}

rtems_shell_cmd_t rtems_shell_DATE_Command = {
  "date",                        /* name */
  "date",                        /* usage */
  "misc",                        /* topic */
  rtems_shell_main_date,         /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
