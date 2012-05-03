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
 *  COPYRIGHT (c) 1989-2009.
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

void Task_delete_extension(
  rtems_tcb *running_task,
  rtems_tcb *deleted_task
)
{
  char line[80];
  rtems_name name;

  if ( task_number( running_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( running_task->Object.id ) ];
    sprintf( line, "TASK_DELETE - %c%c%c%c TASK_DELETE ",
      (char)((name >> 24) & 0xff),
      (char)((name >> 16) & 0xff),
      (char)((name >> 8) & 0xff),
      (char)(name & 0xff)
    );
    buffered_io_add_string( line );
  }
  if ( task_number( deleted_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( deleted_task->Object.id ) ];
/*
 * FIXME: There should be a public function to
 * convert numeric rtems_names into char arrays
 * c.f. rtems_name_to_characters() in rtems/rtems/support.inl
 * but it's private.
 */
    sprintf( line, "deleting - %c%c%c%c\n",
      (char)((name >> 24) & 0xff),
      (char)((name >> 16) & 0xff),
      (char)((name >> 8) & 0xff),
      (char)(name & 0xff)
    );
    buffered_io_add_string( line );
  }
}
