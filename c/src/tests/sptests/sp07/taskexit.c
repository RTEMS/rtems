/*  Task_exit_extension
 *
 *  This routine is the task exitted user extension.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  rtems_test_exit( 0 );
}
