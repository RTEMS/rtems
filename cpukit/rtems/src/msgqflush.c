/**
 * @file
 *
 * @brief rtems_message_queue_flush
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

rtems_status_code rtems_message_queue_flush(
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
    return _Message_queue_MP_Flush( id, count );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  *count = _CORE_message_queue_Flush(
    &the_message_queue->message_queue,
    &queue_context
  );
  return RTEMS_SUCCESSFUL;
}
