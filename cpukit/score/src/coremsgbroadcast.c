/**
 *  @file
 *
 *  @brief Broadcast a Message to the Message Queue
 *  @ingroup RTEMSScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/objectimpl.h>

Status_Control _CORE_message_queue_Broadcast(
  CORE_message_queue_Control *the_message_queue,
  const void                 *buffer,
  size_t                      size,
  uint32_t                   *count,
  Thread_queue_Context       *queue_context
)
{
  Thread_Control             *the_thread;
  uint32_t                    number_broadcasted;

  if ( size > the_message_queue->maximum_message_size ) {
    _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
    return STATUS_MESSAGE_INVALID_SIZE;
  }

  number_broadcasted = 0;

  _CORE_message_queue_Acquire_critical( the_message_queue, queue_context );

  while (
    ( the_thread =
      _CORE_message_queue_Dequeue_receiver(
        the_message_queue,
        buffer,
        size,
        0,
        queue_context
      )
    )
  ) {
    number_broadcasted += 1;

    _CORE_message_queue_Acquire( the_message_queue, queue_context );
  }

  _CORE_message_queue_Release( the_message_queue, queue_context );

  *count = number_broadcasted;
  return STATUS_SUCCESSFUL;
}
