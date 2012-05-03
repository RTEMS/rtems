/*  Task_1
 *
 *  This routine serves as a test task.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include <signal.h>

void *Task_1(
  void *argument
)
{
  int seconds;

  printf( "Task_1: sleeping for 5 seconds\n" );

  seconds = sleep( 5 );
  printf( "Task_1: %d seconds left\n", seconds );
  rtems_test_assert( seconds );

     /* switch to Init */

  printf( "Task_1: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
