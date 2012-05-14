/*
 *  Message Queue Manager - rtems_message_queue_send
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

/*
 *
 *  rtems_message_queue_send
 *
 *  This routine implements the directive rtems_message_queue_send.  It sends a
 *  message to the specified message queue.
 *
 *  Input parameters:
 *    id     - pointer to message queue
 *    buffer - pointer to message buffer
 *    size   - size of message to send
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

#if defined(RTEMS_MULTIPROCESSING)
#define MESSAGE_QUEUE_MP_HANDLER _Message_queue_Core_message_queue_mp_support
#else
#define MESSAGE_QUEUE_MP_HANDLER NULL
#endif

rtems_status_code rtems_message_queue_send(
  rtems_id    id,
  const void *buffer,
  size_t      size
)
{
  register Message_queue_Control  *the_message_queue;
  Objects_Locations                location;
  CORE_message_queue_Status        status;

  if ( !buffer )
    return RTEMS_INVALID_ADDRESS;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      status = _CORE_message_queue_Send(
        &the_message_queue->message_queue,
        buffer,
        size,
        id,
        MESSAGE_QUEUE_MP_HANDLER,
        false,   /* sender does not block */
        0        /* no timeout */
      );

      _Thread_Enable_dispatch();

      /*
       *  Since this API does not allow for blocking sends, we can directly
       *  return the returned status.
       */

      return _Message_queue_Translate_core_message_queue_return_code(status);

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _Message_queue_MP_Send_request_packet(
        MESSAGE_QUEUE_MP_SEND_REQUEST,
        id,
        buffer,
        &size,
        0,                               /* option_set */
        MPCI_DEFAULT_TIMEOUT
      );
      break;
#endif

    case OBJECTS_ERROR:
      break;
  }
  return RTEMS_INVALID_ID;
}
