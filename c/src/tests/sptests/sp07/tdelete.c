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
