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
 *  _POSIX_Message_queue_Delete
 */
 
void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq
)
{
  if ( !the_mq->linked && !the_mq->open_count ) {
      _Objects_Close( &_POSIX_Message_queue_Information, &the_mq->Object );
 
      _CORE_message_queue_Close(
        &the_mq->Message_queue,
        NULL,        /* no MP support */
        CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED
      );

    _POSIX_Message_queue_Free( the_mq );
 
#if 0 && defined(RTEMS_MULTIPROCESSING)
    if ( the_mq->process_shared == PTHREAD_PROCESS_SHARED ) {
 
      _Objects_MP_Close(
        &_POSIX_Message_queue_Information,
        the_mq->Object.id
      );
 
      _POSIX_Message_queue_MP_Send_process_packet(
        POSIX_MESSAGE_QUEUE_MP_ANNOUNCE_DELETE,
        the_mq->Object.id,
        0,                         /* Not used */
        0                          /* Not used */
      );
    }
#endif
 
  }
}

