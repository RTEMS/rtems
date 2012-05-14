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
 *  COPYRIGHT (c) 1989-2007.
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
 *  15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
)
{
  POSIX_Message_queue_Control_fd *the_mq_fd;
  CORE_message_queue_Control     *the_core_mq;
  Objects_Locations               location;

  if ( !mqstat )
    rtems_set_errno_and_return_minus_one( EINVAL );

  the_mq_fd = _POSIX_Message_queue_Get_fd( mqdes, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      the_core_mq = &the_mq_fd->Queue->Message_queue;

      /*
       *  Return the old values.
       */

      if ( omqstat ) {
        omqstat->mq_flags   = the_mq_fd->oflag;
        omqstat->mq_msgsize = the_core_mq->maximum_message_size;
        omqstat->mq_maxmsg  = the_core_mq->maximum_pending_messages;
        omqstat->mq_curmsgs = the_core_mq->number_of_pending_messages;
      }

      the_mq_fd->oflag = mqstat->mq_flags;
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
