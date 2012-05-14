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
 *  COPYRIGHT (c) 1989-2011.
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
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
)
{
  POSIX_Message_queue_Control          *the_mq;
  POSIX_Message_queue_Control_fd       *the_mq_fd;
  Objects_Locations                     location;

  if ( !mqstat )
    rtems_set_errno_and_return_minus_one( EINVAL );

  the_mq_fd = _POSIX_Message_queue_Get_fd( mqdes, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      the_mq = the_mq_fd->Queue;

      /*
       *  Return the old values.
       */
      mqstat->mq_flags   = the_mq_fd->oflag;
      mqstat->mq_msgsize = the_mq->Message_queue.maximum_message_size;
      mqstat->mq_maxmsg  = the_mq->Message_queue.maximum_pending_messages;
      mqstat->mq_curmsgs = the_mq->Message_queue.number_of_pending_messages;

      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EBADF );
}
