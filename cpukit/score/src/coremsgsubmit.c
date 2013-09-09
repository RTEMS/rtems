/**
 * @file
 *
 * @brief CORE Message Queue Submit
 *
 * @ingroup ScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>

CORE_message_queue_Status _CORE_message_queue_Submit(
  CORE_message_queue_Control                *the_message_queue,
  Thread_Control                            *executing,
  const void                                *buffer,
  size_t                                     size,
  Objects_Id                                 id,
  #if defined(RTEMS_MULTIPROCESSING)
    CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  #else
    CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support  __attribute__((unused)),
  #endif
  CORE_message_queue_Submit_types            submit_type,
  bool                                       wait,
  Watchdog_Interval                          timeout
)
{
  CORE_message_queue_Buffer_control   *the_message;
  Thread_Control                      *the_thread;

  if ( size > the_message_queue->maximum_message_size ) {
    return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;
  }

  /*
   *  Is there a thread currently waiting on this message queue?
   */
  if ( the_message_queue->number_of_pending_messages == 0 ) {
    the_thread = _Thread_queue_Dequeue( &the_message_queue->Wait_queue );
    if ( the_thread ) {
      _CORE_message_queue_Copy_buffer(
        buffer,
        the_thread->Wait.return_argument_second.mutable_object,
        size
      );
      *(size_t *) the_thread->Wait.return_argument = size;
      the_thread->Wait.count = (uint32_t) submit_type;

      #if defined(RTEMS_MULTIPROCESSING)
        if ( !_Objects_Is_local_id( the_thread->Object.id ) )
          (*api_message_queue_mp_support) ( the_thread, id );
      #endif
      return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
    }
  }

  /*
   *  No one waiting on the message queue at this time, so attempt to
   *  queue the message up for a future receive.
   */
  the_message =
      _CORE_message_queue_Allocate_message_buffer( the_message_queue );
  if ( the_message ) {
    _CORE_message_queue_Copy_buffer(
      buffer,
      the_message->Contents.buffer,
      size
    );
    the_message->Contents.size = size;
    _CORE_message_queue_Set_message_priority( the_message, submit_type );

    _CORE_message_queue_Insert_message(
       the_message_queue,
       the_message,
       submit_type
    );
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  #if !defined(RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND)
    return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
  #else
    /*
     *  No message buffers were available so we may need to return an
     *  overflow error or block the sender until the message is placed
     *  on the queue.
     */
    if ( !wait ) {
      return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
    }

    /*
     *  Do NOT block on a send if the caller is in an ISR.  It is
     *  deadly to block in an ISR.
     */
    if ( _ISR_Is_in_progress() ) {
      return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED;
    }

    /*
     *  WARNING!! executing should NOT be used prior to this point.
     *  Thus the unusual choice to open a new scope and declare
     *  it as a variable.  Doing this emphasizes how dangerous it
     *  would be to use this variable prior to here.
     */
    {
      ISR_Level        level;

      _ISR_Disable( level );
      _Thread_queue_Enter_critical_section( &the_message_queue->Wait_queue );
      executing->Wait.queue = &the_message_queue->Wait_queue;
      executing->Wait.id = id;
      executing->Wait.return_argument_second.immutable_object = buffer;
      executing->Wait.option = (uint32_t) size;
      executing->Wait.count = submit_type;
      _ISR_Enable( level );

      _Thread_queue_Enqueue(
        &the_message_queue->Wait_queue,
        executing,
        timeout
      );
    }

    return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_WAIT;
  #endif
}
