/**
 * @file
 *
 * @brief RTEMS Message Queue Get Number Pending
 * @ingroup ClassicMessageQueue Message Queues
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>

rtems_status_code rtems_message_queue_get_number_pending(
  rtems_id  id,
  uint32_t *count
)
{
  Message_queue_Control *the_message_queue;
  Objects_Locations      location;
  ISR_lock_Context       lock_context;

  if ( !count )
    return RTEMS_INVALID_ADDRESS;

  the_message_queue = _Message_queue_Get_interrupt_disable(
    id,
    &location,
    &lock_context
  );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_message_queue_Acquire_critical(
        &the_message_queue->message_queue,
        &lock_context
      );
      *count = the_message_queue->message_queue.number_of_pending_messages;
      _CORE_message_queue_Release(
        &the_message_queue->message_queue,
        &lock_context
      );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Get_executing()->Wait.return_argument = count;

      return _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST,
          id,
          0,                               /* buffer not used */
          0,                               /* size */
          0,                               /* option_set not used */
          MPCI_DEFAULT_TIMEOUT
        );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
