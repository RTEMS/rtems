/*  Task_1_through_3
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
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

#include "system.h"

void *Task_1_through_3(
  void *argument
)
{
  int seconds;

  /* XXX temporary */

  puts( "*** END OF POSIX TEST 1 ***" );
  exit( 0 );

  for ( ; ; ) {
    /* grab the tod */

    /* if we have run longer 35 seconds exit */

    if ( seconds >= 35 ) {
      puts( "*** END OF POSIX TEST 1 ***" );
      exit( 0 );
    }

    /* put our task number and the current time */

    /* delay based on our thread number */
 
  }
}
