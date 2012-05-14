/*
 *  POSIX Message Queue Error Translation
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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/coremsg.h>
#include <rtems/posix/mqueue.h>

/*
 *  _POSIX_Message_queue_Translate_core_message_queue_return_code
 *
 *  Input parameters:
 *    the_message_queue_status - message_queue status code to translate
 *
 *  Output parameters:
 *    status code - translated POSIX status code
 *
 */

static
 int _POSIX_Message_queue_Return_codes[CORE_MESSAGE_QUEUE_STATUS_LAST + 1] = {
  0,                     /* CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL */
  EMSGSIZE,              /* CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE */
  EAGAIN,                /* CORE_MESSAGE_QUEUE_STATUS_TOO_MANY */
  ENOMEM,                /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED */
  EAGAIN,                /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT */
  EBADF,                 /* CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED */
  ETIMEDOUT,             /* CORE_MESSAGE_QUEUE_STATUS_TIMEOUT */
  ENOSYS                 /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_WAIT */
};


int _POSIX_Message_queue_Translate_core_message_queue_return_code(
  uint32_t   the_message_queue_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_message_queue_status > CORE_MESSAGE_QUEUE_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Message_queue_Return_codes[the_message_queue_status];
}
