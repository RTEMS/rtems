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
 *  $Id$
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

/*PAGE
 *
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
  Watchdog_Interval   timeout
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
  unsigned32                            length_out;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      rtems_set_errno_and_return_minus_one( EBADF );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      rtems_set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      if ( (the_mq->oflag & O_ACCMODE) == O_WRONLY ) {
        _Thread_Enable_dispatch();
        rtems_set_errno_and_return_minus_one( EBADF );
      }

      if ( msg_len < the_mq->Message_queue.maximum_message_size ) {
        _Thread_Enable_dispatch();
        rtems_set_errno_and_return_minus_one( EMSGSIZE );
      }
 
      /*
       *  Now if something goes wrong, we return a "length" of -1
       *  to indicate an error.
       */

      length_out = -1;

      _CORE_message_queue_Seize(
        &the_mq->Message_queue,
        mqdes,
        msg_ptr,
        &length_out,
        (the_mq->oflag & O_NONBLOCK) ? FALSE : TRUE,
        timeout
      );
      
      _Thread_Enable_dispatch();
      *msg_prio =
        _POSIX_Message_queue_Priority_from_core(_Thread_Executing->Wait.count);

      if ( !_Thread_Executing->Wait.return_code )
        return length_out;

      rtems_set_errno_and_return_minus_one(
        _POSIX_Message_queue_Translate_core_message_queue_return_code(
          _Thread_Executing->Wait.return_code
        )
      );
  }
  return POSIX_BOTTOM_REACHED();
}

