/*  Task_delete_extension
 *
 *  This routine is the delete user extension.
 *
 *  Input parameters:
 *    running_task - pointer to running TCB
 *    deleted_task - pointer to TCB deleted
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

rtems_extension Task_delete_extension(
  rtems_tcb *running_task,
  rtems_tcb *deleted_task
)
{
  if ( task_number( running_task->Object.id ) > 0 ) {
    puts_nocr( "TASK_DELETE - " );
    put_name( Task_name[ task_number( running_task->Object.id ) ], FALSE );
  }
  if ( task_number( deleted_task->Object.id ) > 0 ) {
    puts_nocr( " deleting " );
    put_name( Task_name[ task_number( deleted_task->Object.id ) ], TRUE );
  }
}
