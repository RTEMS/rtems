/*
 *  POSIX Message Queue Error Translation 
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
#include <rtems/score/interr.h>


/*PAGE
 *
 *  _POSIX_Message_queue_Translate_core_message_queue_return_code
 *
 *  Input parameters:
 *    the_message_queue_status - message_queue status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated POSIX status code
 *
 */
 
int _POSIX_Message_queue_Translate_core_message_queue_return_code(
  unsigned32 the_message_queue_status
)
{
  switch ( the_message_queue_status ) {
    case  CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL:
      return 0;

      /*
       *  Bad message size
       */
    case  CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return EMSGSIZE;

      /*
       *  Queue is full of pending messages.
       */
    case  CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return EAGAIN;

      /*
       *  Out of message buffers to queue pending message
       */
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED:
      return ENOMEM;

      /*
       *  No message available on receive poll
       */
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return EAGAIN;

      /*
       *  Queue was deleted while thread blocked on it.
       */
    case CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED:
      return EBADF;

      /*
       *  POSIX Real-Time Extensions add timeouts to send and receive. 
       */
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return ETIMEDOUT;

      /*
       *  RTEMS POSIX API implementation does not support multiprocessing.
       */
    case THREAD_STATUS_PROXY_BLOCKING:
      return ENOSYS;
  }
  _Internal_error_Occurred(
    INTERNAL_ERROR_POSIX_API,
    TRUE,
    the_message_queue_status
  );
  return POSIX_BOTTOM_REACHED();
}
