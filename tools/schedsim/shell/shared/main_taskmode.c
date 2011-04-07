/*
 *  Task Delete Shell Command Implmentation
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

#define __need_getopt_newlib
#include <newlib/getopt.h>

#include <stdio.h>

#include <rtems.h>
#include "shell.h"
#include <rtems/stringto.h>
#include <schedsim_shell.h>
#include <rtems/error.h>

void print_mode(
  const char *prefix,
  rtems_mode  mode
)
{
  fprintf(
    stderr,
    "%sPreemption: %s Timeslicing: %s\n",
    prefix,
    ((mode & RTEMS_NO_PREEMPT) ? "no" : "yes"),
    ((mode & RTEMS_TIMESLICE) ? "yes" : "no")
  );
}

int rtems_shell_main_task_mode(
  int   argc,
  char *argv[]
)
{
  rtems_status_code  status;
  rtems_mode         mode;
  rtems_mode         mask;
  rtems_mode         old;
  struct getopt_data getopt_reent;
  char               option;

  CHECK_RTEMS_IS_UP();

  mode = 0;
  mask = 0;
  memset(&getopt_reent, 0, sizeof(getopt_data));
  while ( (option = getopt_r( argc, argv, "tTpP", &getopt_reent)) != -1 ) {
    switch (option) {
      case 't':
        mask |= RTEMS_TIMESLICE_MASK;
        mode  = (mode & ~RTEMS_TIMESLICE_MASK) | RTEMS_NO_TIMESLICE;
        break;
      case 'T':
        mask |= RTEMS_TIMESLICE_MASK;
        mode  = (mode & ~RTEMS_TIMESLICE_MASK) | RTEMS_TIMESLICE;
        break;
      case 'p':
        mask |= RTEMS_PREEMPT_MASK;
        mode  = (mode & ~RTEMS_PREEMPT_MASK) | RTEMS_NO_PREEMPT;
        break;
      case 'P':
        mask |= RTEMS_PREEMPT_MASK;
        mode  = (mode & ~RTEMS_PREEMPT_MASK) | RTEMS_PREEMPT;
        break;
      default:
        fprintf( stderr, "%s: Usage [-tTpP]\n", argv[0] );
        return -1;
    }
  }

  /*
   *  Now change the task mode
   */
  status = rtems_task_mode( mode, mask, &old );
  if ( status != RTEMS_SUCCESSFUL ) {
    fprintf(
      stderr,
      "Task Mode returned %s\n",
      rtems_status_text( status )
    ); 
    return -1;
  }

  print_mode( "Previous Mode: ", old );
  if ( mask ) {
    (void) rtems_task_mode( RTEMS_CURRENT_MODE ,  RTEMS_CURRENT_MODE, &old );
    print_mode( "Current Mode:  ", mode );
  }
  
  return 0;
}

rtems_shell_cmd_t rtems_shell_TASK_MODE_Command = {
  "task_mode",                     /* name */
  "task_mode [-tTpP]",             /* usage */
  "rtems",                         /* topic */
  rtems_shell_main_task_mode,      /* command */
  NULL,                            /* alias */
  NULL                             /* next */
};
