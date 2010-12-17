/*
 *  Thread Executing Shell Command Implmentation
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
#include <rtems/stringto.h>
#include <schedsim_shell.h>
#include <rtems/error.h>

int rtems_shell_main_executing(
  int   argc,
  char *argv[]
)
{
  PRINT_EXECUTING();
  return 0;
}

rtems_shell_cmd_t rtems_shell_TASK_EXECUTING_Command = {
  "executing",                 /* name */
  "executing",                 /* usage */
  "rtems",                     /* topic */
  rtems_shell_main_executing,  /* command */
  NULL,                        /* alias */
  NULL                         /* next */
};
