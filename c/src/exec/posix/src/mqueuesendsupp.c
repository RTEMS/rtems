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

#include <stdarg.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/seterr.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_Message_queue_Send_support
 */
 
int _POSIX_Message_queue_Send_support(
  mqd_t               mqdes,
  const char         *msg_ptr,
  unsigned32          msg_len,
  Priority_Control    msg_prio,
  Watchdog_Interval   timeout
)
{
  register POSIX_Message_queue_Control *the_mq;
  Objects_Locations                     location;
 
  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      /* XXX must add support for timeout and priority */
      _CORE_message_queue_Send(
        &the_mq->Message_queue,
        (void *) msg_ptr,
        msg_len,
        mqdes,
#if defined(RTEMS_MULTIPROCESSING)
        NULL       /* XXX _POSIX_Message_queue_Core_message_queue_mp_support*/
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return _Thread_Executing->Wait.return_code;
  }
  return POSIX_BOTTOM_REACHED();
}

