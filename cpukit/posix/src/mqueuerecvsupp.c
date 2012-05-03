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
 *  _POSIX_Message_queue_Receive_support
 *
 *  NOTE: XXX Document how size, priority, length, and the buffer go
 *        through the layers.
 */

ssize_t _POSIX_Message_queue_Receive_support(
  mqd_t               mqdes,
  char               *msg_ptr,
  size_t              msg_len,
  unsigned int       *msg_prio,
  bool                wait,
  Watchdog_Interval   timeout
)
{
  POSIX_Message_queue_Control     *the_mq;
  POSIX_Message_queue_Control_fd  *the_mq_fd;
  Objects_Locations                location;
  size_t                           length_out;
  bool                             do_wait;

  the_mq_fd = _POSIX_Message_queue_Get_fd( mqdes, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( (the_mq_fd->oflag & O_ACCMODE) == O_WRONLY ) {
        _Thread_Enable_dispatch();
        rtems_set_errno_and_return_minus_one( EBADF );
      }

      the_mq = the_mq_fd->Queue;

      if ( msg_len < the_mq->Message_queue.maximum_message_size ) {
        _Thread_Enable_dispatch();
        rtems_set_errno_and_return_minus_one( EMSGSIZE );
      }

      /*
       *  Now if something goes wrong, we return a "length" of -1
       *  to indicate an error.
       */

      length_out = -1;

      /*
       *  A timed receive with a bad time will do a poll regardless.
       */
      if ( wait )
        do_wait = (the_mq_fd->oflag & O_NONBLOCK) ? false : true;
      else
        do_wait = wait;

      /*
       *  Now perform the actual message receive
       */
      _CORE_message_queue_Seize(
        &the_mq->Message_queue,
        mqdes,
        msg_ptr,
        &length_out,
        do_wait,
        timeout
      );

      _Thread_Enable_dispatch();
      if (msg_prio) {
        *msg_prio = _POSIX_Message_queue_Priority_from_core(
             _Thread_Executing->Wait.count
          );
      }

      if ( !_Thread_Executing->Wait.return_code )
        return length_out;

      rtems_set_errno_and_return_minus_one(
        _POSIX_Message_queue_Translate_core_message_queue_return_code(
          _Thread_Executing->Wait.return_code
        )
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EBADF );
}
