/*  Task_3
 *
 *  This routine serves as a test task.  Its only purpose in life is to
 *  generate the error where a message queue is deleted while a task
 *  is waiting there for a message.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2007.
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

rtems_task Task_3(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  long              buffer[ 4 ];
  size_t            size;

  puts( "TA3 - rtems_message_queue_receive - Q 1 - RTEMS_WAIT FOREVER" );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_OBJECT_WAS_DELETED,
    "rtems_message_queue_receive waiting to be deleted"
  );
  puts(
    "TA3 - rtems_message_queue_receive - woke up with RTEMS_OBJECT_WAS_DELETED"
  );

  puts( "TA3 - rtems_task_delete - delete self - RTEMS_SUCCESSFUL" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of TA3" );
}
