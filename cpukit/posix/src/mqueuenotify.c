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
#include <sys/types.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Message_queue_Notify_handler
 *
 */

static void _POSIX_Message_queue_Notify_handler(
  void    *user_data
)
{
  POSIX_Message_queue_Control *the_mq;

  the_mq = user_data;

  kill( getpid(), the_mq->notification.sigev_signo );

  _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL, NULL );
}

/*
 *  15.2.6 Notify Process that a Message is Available on a Queue,
 *         P1003.1b-1993, p. 280
 */

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
)
{
  POSIX_Message_queue_Control    *the_mq;
  POSIX_Message_queue_Control_fd *the_mq_fd;
  Objects_Locations               location;

  the_mq_fd = _POSIX_Message_queue_Get_fd( mqdes, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      the_mq = the_mq_fd->Queue;

      if ( notification ) {
        if ( _CORE_message_queue_Is_notify_enabled( &the_mq->Message_queue ) ) {
          _Thread_Enable_dispatch();
          rtems_set_errno_and_return_minus_one( EBUSY );
        }

        _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL, NULL );

        the_mq->notification = *notification;

        _CORE_message_queue_Set_notify(
          &the_mq->Message_queue,
          _POSIX_Message_queue_Notify_handler,
          the_mq
        );
      } else {

        _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL, NULL );

      }

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
