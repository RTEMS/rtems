/*
 *  Message Queue Manager
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

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

/*PAGE
 *
 *  _Message_queue_Translate_core_message_queue_return_code
 *
 *  Input parameters:
 *    the_message_queue_status - message_queue status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  unsigned32 the_message_queue_status
)
{
  switch ( the_message_queue_status ) {
    case  CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL:
      return RTEMS_SUCCESSFUL;
    case  CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return RTEMS_INVALID_SIZE;
    case  CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return RTEMS_TOO_MANY;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED:
      return RTEMS_UNSATISFIED;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return RTEMS_UNSATISFIED;
    case CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED:
      return RTEMS_OBJECT_WAS_DELETED;
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return RTEMS_TIMEOUT;
    case THREAD_STATUS_PROXY_BLOCKING:
      return RTEMS_PROXY_BLOCKING;
  }
  _Internal_error_Occurred(         /* XXX */
    INTERNAL_ERROR_RTEMS_API,
    TRUE,
    the_message_queue_status
  );
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
