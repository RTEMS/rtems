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

rtems_extension Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
)
{
  if ( task_number( created_task->Object.id ) > 0 ) {
    puts_nocr( "TASK_CREATE - " );
    put_name( Task_name[ task_number( created_task->Object.id ) ], FALSE );
    puts( " - created." );
  }
}
