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

rtems_extension Task_start_extension(
  rtems_tcb *unused,
  rtems_tcb *started_task
)
{
  char line[80];
  rtems_name name;

  if ( task_number( started_task->Object.id ) > 0 ) {
    name = Task_name[ task_number( started_task->Object.id ) ];
    sprintf( line, "TASK_START - %c%c%c%c - started\n",
      (name >> 24) & 0xff, 
      (name >> 16) & 0xff,
      (name >> 8) & 0xff,
      name & 0xff
    );
    buffered_io_add_string( line );
  }
}
