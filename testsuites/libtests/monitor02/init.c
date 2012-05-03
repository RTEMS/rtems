/*
 *  This is a simple test whose only purpose is to start the Monitor
 *  task.  The Monitor task can be used to obtain information about
 *  a variety of RTEMS objects.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "bsp.h"
#include "system.h"

#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#define MAX_ARGS 128

char        *Commands[] = {
  "task",
  "task 32",
  NULL
};
rtems_task Init(
  rtems_task_argument argument
)
{
#if !BSP_SMALL_MEMORY
  int                i;
  int                argc;
  char               *argv[MAX_ARGS];
  char               tmp[256];

  puts( "\n*** START OF MONITOR02 ***\n");

  for (i=0; i < MAX_ARGS && Commands[i] ; i++) {
    strcpy( tmp, Commands[i] );
    if (!rtems_shell_make_args(tmp, &argc, argv, MAX_ARGS) ) {
      printf( "===> %s\n", Commands[i] );
      rtems_shell_main_monitor(argc, argv);
    }
  }
  puts( "\n*** END OF MONITOR02 ***\n");
#else
  puts( "TARGET MEMORY TOO SMALL TO RUN\n\n");
#endif
  rtems_test_exit(0);
}
