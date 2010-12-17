/*
 *  Initialize RTEMS Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
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
#include "shell.h"
#include <schedsim_shell.h>

int rtems_shell_main_rtems_init(
  int   argc,
  char *argv[]
)
{
  //
  // Initialize RTEMS
  //
  rtems_initialize_data_structures();
  return 0;
}

rtems_shell_cmd_t rtems_shell_RTEMS_INIT_Command = {
  "rtems_init",                 /* name */
  "rtems_init",                 /* usage */
  "rtems",                      /* topic */
  rtems_shell_main_rtems_init,  /* command */
  NULL,                         /* alias */
  NULL                          /* next */
};
