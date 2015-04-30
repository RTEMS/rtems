/**
 *  @file
 *
 *  @brief Broadcast a Message to the Message Queue
 *  @ingroup ScoreMessageQueue
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

CORE_message_queue_Status _CORE_message_queue_Broadcast(
  CORE_message_queue_Control                *the_message_queue,
  const void                                *buffer,
  size_t                                     size,
  #if defined(RTEMS_MULTIPROCESSING)
    Objects_Id                                 id,
    CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  #else
    Objects_Id                                 id __attribute__((unused)),
    CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support __attribute__((unused)),
  #endif
  uint32_t                                  *count,
  ISR_lock_Context                          *lock_context
)
{
  Thread_Control             *the_thread;
  uint32_t                    number_broadcasted;

  if ( size > the_message_queue->maximum_message_size ) {
    _ISR_lock_ISR_enable( lock_context );
    return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;
  }

  number_broadcasted = 0;

  _CORE_message_queue_Acquire_critical( the_message_queue, lock_context );

  while (
    ( the_thread =
      _CORE_message_queue_Dequeue_receiver(
        the_message_queue,
        buffer,
        size,
        0,
        lock_context
      )
    )
  ) {
    number_broadcasted += 1;

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_message_queue_mp_support) ( the_thread, id );
#endif

    _CORE_message_queue_Acquire( the_message_queue, lock_context );
  }

  _CORE_message_queue_Release( the_message_queue, lock_context );

  *count = number_broadcasted;
  return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
}
