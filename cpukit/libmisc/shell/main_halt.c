/*
 *  Halt Command Implementation
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

int rtems_shell_main_halt(
  int   argc __attribute__((unused)),
  char *argv[] __attribute__((unused))
)
{
  exit(0);
  return 0;
}

rtems_shell_cmd_t rtems_shell_HALT_Command = {
  "halt",                                    /* name */
  "halt",                                    /* usage */
  "rtems",                                   /* topic */
  rtems_shell_main_halt,                     /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
