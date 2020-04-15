/**
 *  @file
 *
 *  @brief Seize a Message from the Message Queue
 *  @ingroup RTEMSScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/statesimpl.h>

Status_Control _CORE_message_queue_Seize(
  CORE_message_queue_Control *the_message_queue,
  Thread_Control             *executing,
  void                       *buffer,
  size_t                     *size_p,
  bool                        wait,
  Thread_queue_Context       *queue_context
)
{
  CORE_message_queue_Buffer_control *the_message;

  the_message = _CORE_message_queue_Get_pending_message( the_message_queue );
  if ( the_message != NULL ) {
    the_message_queue->number_of_pending_messages -= 1;

    *size_p = the_message->Contents.size;
    executing->Wait.count =
      _CORE_message_queue_Get_message_priority( the_message );
    _CORE_message_queue_Copy_buffer(
      the_message->Contents.buffer,
      buffer,
      *size_p
    );

    #if !defined(RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND)
      /*
       *  There is not an API with blocking sends enabled.
       *  So return immediately.
       */
      _CORE_message_queue_Free_message_buffer(the_message_queue, the_message);
      _CORE_message_queue_Release( the_message_queue, queue_context );
      return STATUS_SUCCESSFUL;
    #else
    {
      Thread_Control   *the_thread;

      /*
       *  There could be a thread waiting to send a message.  If there
       *  is not, then we can go ahead and free the buffer.
       *
       *  NOTE: If we note that the queue was not full before this receive,
       *  then we can avoid this dequeue.
       */
      the_thread = _Thread_queue_First_locked(
        &the_message_queue->Wait_queue,
        the_message_queue->operations
      );
      if ( the_thread == NULL ) {
        _CORE_message_queue_Free_message_buffer(
          the_message_queue,
          the_message
        );
        _CORE_message_queue_Release( the_message_queue, queue_context );
        return STATUS_SUCCESSFUL;
      }

      /*
       *  There was a thread waiting to send a message.  This code
       *  puts the messages in the message queue on behalf of the
       *  waiting task.
       */
      _CORE_message_queue_Insert_message(
        the_message_queue,
        the_message,
        the_thread->Wait.return_argument_second.immutable_object,
        (size_t) the_thread->Wait.option,
        (CORE_message_queue_Submit_types) the_thread->Wait.count
      );
      _Thread_queue_Extract_critical(
        &the_message_queue->Wait_queue.Queue,
        the_message_queue->operations,
        the_thread,
        queue_context
      );
      return STATUS_SUCCESSFUL;
    }
    #endif
  }

  if ( !wait ) {
    _CORE_message_queue_Release( the_message_queue, queue_context );
    return STATUS_UNSATISFIED;
  }

  executing->Wait.return_argument_second.mutable_object = buffer;
  executing->Wait.return_argument = size_p;
  /* Wait.count will be filled in with the message priority */

  _Thread_queue_Context_set_thread_state(
    queue_context,
    STATES_WAITING_FOR_MESSAGE
  );
  _Thread_queue_Enqueue(
    &the_message_queue->Wait_queue.Queue,
    the_message_queue->operations,
    executing,
    queue_context
  );
  return _Thread_Wait_get_status( executing );
}
