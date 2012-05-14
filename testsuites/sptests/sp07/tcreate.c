/*  Task_create_extension
 *
 *  This routine is the tcreate user extension.
 *
 *  Input parameters:
 *    unused       - pointer to currently running TCB
 *    created_task - pointer to new TCB being created
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

bool Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
)
{
  char line[80];
  rtems_name name;

  if ( task_number( created_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( created_task->Object.id ) ];
/*
 * FIXME: There should be a public function to
 * convert numeric rtems_names into char arrays
 * c.f. rtems_name_to_characters() in rtems/rtems/support.inl
 * but it's private.
 */
    sprintf( line, "TASK_CREATE - %c%c%c%c - created\n",
      (char)((name >> 24) & 0xff),
      (char)((name >> 16) & 0xff),
      (char)((name >> 8) & 0xff),
      (char)(name & 0xff)
    );
    buffered_io_add_string( line );
  }
  return TRUE;
}
