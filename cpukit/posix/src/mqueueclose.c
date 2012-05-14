/*
 *  NOTE:  The structure of the routines is identical to that of POSIX
 *         Message_queues to leave the option of having unnamed message
 *         queues at a future date.  They are currently not part of the
 *         POSIX standard but unnamed message_queues are.  This is also
 *         the reason for the apparently unnecessary tracking of
 *         the process_shared attribute.  [In addition to the fact that
 *         it would be trivial to add pshared to the mq_attr structure
 *         and have process private message queues.]
 *
 *         This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *         time.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*
 *
 *  15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */

int mq_close(
  mqd_t  mqdes
)
{
  POSIX_Message_queue_Control    *the_mq;
  POSIX_Message_queue_Control_fd *the_mq_fd;
  Objects_Locations               location;

  the_mq_fd = _POSIX_Message_queue_Get_fd( mqdes, &location );
  if ( location == OBJECTS_LOCAL ) {
      /* OBJECTS_LOCAL:
       *
       *  First update the actual message queue to reflect this descriptor
       *  being disassociated.  This may result in the queue being really
       *  deleted.
       */

      the_mq = the_mq_fd->Queue;
      the_mq->open_count -= 1;
      _POSIX_Message_queue_Delete( the_mq );

      /*
       *  Now close this file descriptor.
       */

      _Objects_Close(
        &_POSIX_Message_queue_Information_fds, &the_mq_fd->Object );
      _POSIX_Message_queue_Free_fd( the_mq_fd );

      _Thread_Enable_dispatch();
      return 0;
   }

   /*
    *  OBJECTS_REMOTE:
    *  OBJECTS_ERROR:
    */
   rtems_set_errno_and_return_minus_one( EBADF );
}
