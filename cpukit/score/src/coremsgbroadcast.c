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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/thread.h>

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
  uint32_t                                  *count
)
{
  Thread_Control          *the_thread;
  uint32_t                 number_broadcasted;
  Thread_Wait_information *waitp;

  if ( size > the_message_queue->maximum_message_size ) {
    return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;
  }

  /*
   *  If there are pending messages, then there can't be threads
   *  waiting for us to send them a message.
   *
   *  NOTE: This check is critical because threads can block on
   *        send and receive and this ensures that we are broadcasting
   *        the message to threads waiting to receive -- not to send.
   */

  if ( the_message_queue->number_of_pending_messages != 0 ) {
    *count = 0;
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  /*
   *  There must be no pending messages if there is a thread waiting to
   *  receive a message.
   */
  number_broadcasted = 0;
  while ((the_thread =
          _Thread_queue_Dequeue(&the_message_queue->Wait_queue))) {
    waitp = &the_thread->Wait;
    number_broadcasted += 1;

    _CORE_message_queue_Copy_buffer(
      buffer,
      waitp->return_argument_second.mutable_object,
      size
    );

    *(size_t *) the_thread->Wait.return_argument = size;

    #if defined(RTEMS_MULTIPROCESSING)
      if ( !_Objects_Is_local_id( the_thread->Object.id ) )
        (*api_message_queue_mp_support) ( the_thread, id );
    #endif

  }
  *count = number_broadcasted;
  return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
}
