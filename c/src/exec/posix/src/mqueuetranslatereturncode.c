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
    case  CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return EMSGSIZE;
    case  CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return EAGAIN;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED:
      return ENOSYS;                  /* XXX */
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return ENOSYS;                  /* XXX */
    case CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED:
      return EBADF;
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return ENOSYS;                  /* XXX */
    case THREAD_STATUS_PROXY_BLOCKING:
      return ENOSYS;                  /* XXX */
  }
  _Internal_error_Occurred(         /* XXX */
    INTERNAL_ERROR_POSIX_API,
    TRUE,
    the_message_queue_status
  );
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
