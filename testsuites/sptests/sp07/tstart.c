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
