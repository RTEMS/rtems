/*
 *  Halt Command Implementation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_shutdown(
  int   argc RTEMS_UNUSED,
  char *argv[] RTEMS_UNUSED
)
{
  fprintf(stdout, "System shutting down at user request\n");
  exit(0);
  return 0;
}

rtems_shell_cmd_t rtems_shell_SHUTDOWN_Command = {
  "shutdown",                                /* name */
  "shutdown",                                /* usage */
  "rtems",                                   /* topic */
  rtems_shell_main_shutdown,                 /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
