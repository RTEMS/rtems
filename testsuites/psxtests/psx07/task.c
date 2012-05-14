/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

#include "system.h"
#include <time.h>
#include <sched.h>

void *Task_1(
  void *argument
)
{
  puts( "Task_1 - exitting" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}

void *Task_2(
  void *argument
)
{
  int i = 0;
  time_t now, start;

  /*
   * sleep long enough to let the init thread join with us.
   */
  usleep(10000);

  /*
   *  Change our priority so we are running at a logically higher
   *  priority than our "ss_high_priority".  This should result in
   *  our replenishment period not touching our priority.
   */

  /*
   *  Consume time so the cpu budget callout will run.
   *
   *  DO NOT BLOCK!!!
   */
  start = time(&start);
  while( i <= 10 ) {
    do {
      now = time(&now);
    } while (start == now);
    start = time(&start);

    printf( "Time elapsed Task_2: %2d (seconds)\n", i++ );
  }

  puts( "Task_2 - exitting" );
  pthread_exit( NULL );
  return NULL;
}
