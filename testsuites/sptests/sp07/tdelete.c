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
  char line[80];
  rtems_name name;

  if ( task_number( running_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( running_task->Object.id ) ];
    sprintf( line, "TASK_DELETE - %c%c%c%c",
      (name >> 24) & 0xff, 
      (name >> 16) & 0xff,
      (name >> 8) & 0xff,
      name & 0xff
    );
    buffered_io_add_string( line );
    puts_nocr( "TASK_DELETE - " );
    put_name( Task_name[ task_number( running_task->Object.id ) ], FALSE );
  }
  if ( task_number( deleted_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( deleted_task->Object.id ) ];
    sprintf( line, "deleting - %c%c%c%c\n",
      (name >> 24) & 0xff, 
      (name >> 16) & 0xff,
      (name >> 8) & 0xff,
      name & 0xff
    );
    buffered_io_add_string( line );
  }
}
