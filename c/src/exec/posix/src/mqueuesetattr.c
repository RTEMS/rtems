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
 *  15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
)
{
  register POSIX_Message_queue_Control *the_mq;
  CORE_message_queue_Control           *the_core_mq;
  Objects_Locations                     location;
  CORE_message_queue_Attributes        *the_mq_attr;
 
  if ( !mqstat )
    rtems_set_errno_and_return_minus_one( EINVAL );

  the_mq = _POSIX_Message_queue_Get( mqdes, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      rtems_set_errno_and_return_minus_one( EBADF );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      rtems_set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:

      the_core_mq = &the_mq->Message_queue;

      /*
       *  Return the old values.
       */

      /* XXX this is the same stuff as is in mq_getattr... and probably */
      /* XXX should be in an inlined private routine */

      if ( omqstat ) {
        omqstat->mq_flags   = the_mq->oflag;
        omqstat->mq_msgsize = the_core_mq->maximum_message_size;
        omqstat->mq_maxmsg  = the_core_mq->maximum_pending_messages;
        omqstat->mq_curmsgs = the_core_mq->number_of_pending_messages;
      }
  
      /*
       *  If blocking was in effect and is not now, then there
       *  may be threads blocked on this message queue which need
       *  to be unblocked to make the state of the message queue
       *  consistent for future use.
       */
 
      the_mq_attr = &the_core_mq->Attributes;

      if ( !(the_mq->oflag & O_NONBLOCK) &&         /* were blocking */
           (mqstat->mq_flags & O_NONBLOCK) ) {      /* and now are not */ 
        _CORE_message_queue_Flush_waiting_threads( the_core_mq );
      }
      
      the_mq->oflag = mqstat->mq_flags;

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

