/*
 *  Message Queue Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 
rtems_status_code _Message_queue_Translate_core_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL */
  RTEMS_INVALID_SIZE,       /* CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE */
  RTEMS_TOO_MANY,           /* CORE_MESSAGE_QUEUE_STATUS_TOO_MANY */
  RTEMS_UNSATISFIED,        /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED */
  RTEMS_UNSATISFIED,        /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED */
  RTEMS_TIMEOUT             /* CORE_MESSAGE_QUEUE_STATUS_TIMEOUT */
};

rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  unsigned32 status
)
{
#if defined(RTEMS_MULTIPROCESSING)
  if ( status == THREAD_STATUS_PROXY_BLOCKING )
    return RTEMS_PROXY_BLOCKING;
  else
#endif
  if ( status > CORE_MESSAGE_QUEUE_STATUS_TIMEOUT )
    return RTEMS_INTERNAL_ERROR;
  else
    return _Message_queue_Translate_core_return_code_[status];
}
