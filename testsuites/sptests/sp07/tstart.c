/*  Task_start_extension
 *
 *  This routine is the tstart user extension.
 *
 *  Input parameters:
 *    unused        - pointer to currently running TCB
 *    started_task  - pointer to new TCB being started
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

rtems_extension Task_start_extension(
  rtems_tcb *unused,
  rtems_tcb *started_task
)
{
  if ( task_number( started_task->Object.id ) > 0 ) {
    puts_nocr( "TASK_START - " );
    put_name( Task_name[ task_number( started_task->Object.id ) ], FALSE );
    puts( " - started." );
  }
}
