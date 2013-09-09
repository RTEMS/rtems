/**
 *  @file
 *
 *  @brief Size a Message from the Message Queue
 *  @ingroup ScoreMessageQueue
 */

/*
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

void _CORE_message_queue_Seize(
  CORE_message_queue_Control      *the_message_queue,
  Thread_Control                  *executing,
  Objects_Id                       id,
  void                            *buffer,
  size_t                          *size_p,
  bool                             wait,
  Watchdog_Interval                timeout
)
{
  ISR_Level                          level;
  CORE_message_queue_Buffer_control *the_message;

  executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  the_message = _CORE_message_queue_Get_pending_message( the_message_queue );
  if ( the_message != NULL ) {
    the_message_queue->number_of_pending_messages -= 1;
    _ISR_Enable( level );

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
      return;
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
      the_thread = _Thread_queue_Dequeue( &the_message_queue->Wait_queue );
      if ( !the_thread ) {
        _CORE_message_queue_Free_message_buffer(
          the_message_queue,
          the_message
        );
        return;
      }

      /*
       *  There was a thread waiting to send a message.  This code
       *  puts the messages in the message queue on behalf of the
       *  waiting task.
       */
      _CORE_message_queue_Set_message_priority(
        the_message,
        the_thread->Wait.count
      );
      the_message->Contents.size = (size_t) the_thread->Wait.option;
      _CORE_message_queue_Copy_buffer(
        the_thread->Wait.return_argument_second.immutable_object,
        the_message->Contents.buffer,
        the_message->Contents.size
      );

      _CORE_message_queue_Insert_message(
         the_message_queue,
         the_message,
         _CORE_message_queue_Get_message_priority( the_message )
      );
      return;
    }
    #endif
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_message_queue->Wait_queue );
  executing->Wait.queue = &the_message_queue->Wait_queue;
  executing->Wait.id = id;
  executing->Wait.return_argument_second.mutable_object = buffer;
  executing->Wait.return_argument = size_p;
  /* Wait.count will be filled in with the message priority */
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_message_queue->Wait_queue, executing, timeout );
}
