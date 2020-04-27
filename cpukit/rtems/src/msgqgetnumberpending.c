/**
 * @file
 *
 * @ingroup ClassicMessageQueue Message Queues
 *
 * @brief RTEMS Message Queue Get Number Pending
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>

rtems_status_code rtems_message_queue_get_number_pending(
  rtems_id  id,
  uint32_t *count
)
{
  Message_queue_Control *the_message_queue;
  Thread_queue_Context   queue_context;

  if ( count == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_message_queue = _Message_queue_Get( id, &queue_context );

  if ( the_message_queue == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Message_queue_MP_Get_number_pending( id, count );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _CORE_message_queue_Acquire_critical(
    &the_message_queue->message_queue,
    &queue_context
  );
  *count = the_message_queue->message_queue.number_of_pending_messages;
  _CORE_message_queue_Release(
    &the_message_queue->message_queue,
    &queue_context
  );
  return RTEMS_SUCCESSFUL;
}
