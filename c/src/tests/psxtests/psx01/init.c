/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"

void *POSIX_Init(
  void *argument
)
{
  int        status;
  pthread_t  thread_id;

  puts( "\n\n*** POSIX TEST 1 ***" );

  /* set the time of day */

  /* create the three tasks */

  /* exit */

  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  pthread_exit( NULL );

  /* just to satisfy compilers which think we did not return anything */

  return NULL;
}
