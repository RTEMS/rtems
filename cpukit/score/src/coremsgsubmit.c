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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/isr.h>
#include <rtems/score/statesimpl.h>
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
  Watchdog_Interval                          timeout,
  ISR_lock_Context                          *lock_context
)
{
  CORE_message_queue_Buffer_control *the_message;
  Thread_Control                    *the_thread;

  if ( size > the_message_queue->maximum_message_size ) {
    _ISR_lock_ISR_enable( lock_context );
    return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;
  }

  _CORE_message_queue_Acquire_critical( the_message_queue, lock_context );

  /*
   *  Is there a thread currently waiting on this message queue?
   */

  the_thread = _CORE_message_queue_Dequeue_receiver(
    the_message_queue,
    buffer,
    size,
    submit_type,
    lock_context
  );
  if ( the_thread != NULL ) {
    #if defined(RTEMS_MULTIPROCESSING)
      if ( !_Objects_Is_local_id( the_thread->Object.id ) )
        (*api_message_queue_mp_support) ( the_thread, id );

      _Thread_Dispatch_enable( _Per_CPU_Get() );
    #endif
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  /*
   *  No one waiting on the message queue at this time, so attempt to
   *  queue the message up for a future receive.
   */
  the_message =
      _CORE_message_queue_Allocate_message_buffer( the_message_queue );
  if ( the_message ) {
    the_message->Contents.size = size;
    _CORE_message_queue_Set_message_priority( the_message, submit_type );
    _CORE_message_queue_Copy_buffer(
      buffer,
      the_message->Contents.buffer,
      size
    );

    _CORE_message_queue_Insert_message(
       the_message_queue,
       the_message,
       submit_type
    );
    _CORE_message_queue_Release( the_message_queue, lock_context );
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  #if !defined(RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND)
    _CORE_message_queue_Release( the_message_queue, lock_context );
    return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
  #else
    /*
     *  No message buffers were available so we may need to return an
     *  overflow error or block the sender until the message is placed
     *  on the queue.
     */
    if ( !wait ) {
      _CORE_message_queue_Release( the_message_queue, lock_context );
      return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
    }

    /*
     *  Do NOT block on a send if the caller is in an ISR.  It is
     *  deadly to block in an ISR.
     */
    if ( _ISR_Is_in_progress() ) {
      _CORE_message_queue_Release( the_message_queue, lock_context );
      return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED;
    }

    /*
     *  WARNING!! executing should NOT be used prior to this point.
     *  Thus the unusual choice to open a new scope and declare
     *  it as a variable.  Doing this emphasizes how dangerous it
     *  would be to use this variable prior to here.
     */
    executing->Wait.id = id;
    executing->Wait.return_argument_second.immutable_object = buffer;
    executing->Wait.option = (uint32_t) size;
    executing->Wait.count = submit_type;

    _Thread_queue_Enqueue_critical(
      &the_message_queue->Wait_queue.Queue,
      the_message_queue->Wait_queue.operations,
      executing,
      STATES_WAITING_FOR_MESSAGE,
      timeout,
      CORE_MESSAGE_QUEUE_STATUS_TIMEOUT,
      lock_context
    );
    #if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch_enable( _Per_CPU_Get() );
    #endif

    return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_WAIT;
  #endif
}
