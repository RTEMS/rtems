/*  Task_exit_extension
 *
 *  This routine is the task exitted user extension.
 *
 *  Input parameters:   NONE
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

rtems_extension Task_exit_extension(
  rtems_tcb *running_task
)
{
  if ( task_number( running_task->Object.id ) > 0 ) {
    puts_nocr( "RTEMS_TASK_EXITTED - extension invoked for " );
    put_name( Task_name[ task_number( running_task->Object.id ) ], TRUE );
  }

  puts("*** END OF TEST 7 ***" );
  exit( 0 );
}
