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
 *
 *  $Id$
 */

#include "system.h"

rtems_boolean Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
)
{
  char line[80];
  rtems_name name;

  if ( task_number( created_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( created_task->Object.id ) ];
    sprintf( line, "TASK_CREATE - %c%c%c%c - created\n",
      (name >> 24) & 0xff, 
      (name >> 16) & 0xff,
      (name >> 8) & 0xff,
      name & 0xff
    );
    buffered_io_add_string( line );
  }
  return TRUE;
}
