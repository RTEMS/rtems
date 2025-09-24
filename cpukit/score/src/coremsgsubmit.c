/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Submit().
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/isr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

Status_Control _CORE_message_queue_Submit(
  CORE_message_queue_Control     *the_message_queue,
  Thread_Control                 *executing,
  const void                     *buffer,
  size_t                          size,
  CORE_message_queue_Submit_types submit_type,
  bool                            wait,
  Thread_queue_Context           *queue_context
)
{
  CORE_message_queue_Buffer *the_message;
  Thread_Control            *the_thread;

  if ( size > the_message_queue->maximum_message_size ) {
    _CORE_message_queue_Release( the_message_queue, queue_context );
    return STATUS_MESSAGE_INVALID_SIZE;
  }

  /*
   *  Is there a thread currently waiting on this message queue?
   */

  the_thread = _CORE_message_queue_Dequeue_receiver(
    the_message_queue,
    buffer,
    size,
    submit_type,
    queue_context
  );
  if ( the_thread != NULL ) {
    return STATUS_SUCCESSFUL;
  }

  /*
   *  No one waiting on the message queue at this time, so attempt to
   *  queue the message up for a future receive.
   */
  the_message = _CORE_message_queue_Allocate_message_buffer(
    the_message_queue
  );
  if ( the_message ) {
    _CORE_message_queue_Insert_message(
      the_message_queue,
      the_message,
      buffer,
      size,
      submit_type
    );

#if defined( RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION )
    /*
     *  According to POSIX, does this happen before or after the message
     *  is actually enqueued.  It is logical to think afterwards, because
     *  the message is actually in the queue at this point.
     */
    if (
      the_message_queue->number_of_pending_messages == 1 &&
      the_message_queue->notify_handler != NULL
    ) {
      ( *the_message_queue->notify_handler )(
        the_message_queue,
        queue_context
      );
    } else {
      _CORE_message_queue_Release( the_message_queue, queue_context );
    }
#else
    _CORE_message_queue_Release( the_message_queue, queue_context );
#endif

    return STATUS_SUCCESSFUL;
  }

#if !defined( RTEMS_SCORE_COREMSG_ENABLE_BLOCKING_SEND )
  _CORE_message_queue_Release( the_message_queue, queue_context );
  return STATUS_TOO_MANY;
#else
  /*
   *  No message buffers were available so we may need to return an
   *  overflow error or block the sender until the message is placed
   *  on the queue.
   */
  if ( !wait ) {
    _CORE_message_queue_Release( the_message_queue, queue_context );
    return STATUS_TOO_MANY;
  }

  /*
   *  Do NOT block on a send if the caller is in an ISR.  It is
   *  deadly to block in an ISR.
   */
  if ( _ISR_Is_in_progress() ) {
    _CORE_message_queue_Release( the_message_queue, queue_context );
    return STATUS_MESSAGE_QUEUE_WAIT_IN_ISR;
  }

  /*
   *  WARNING!! executing should NOT be used prior to this point.
   *  Thus the unusual choice to open a new scope and declare
   *  it as a variable.  Doing this emphasizes how dangerous it
   *  would be to use this variable prior to here.
   */
  executing->Wait.return_argument_second.immutable_object = buffer;
  executing->Wait.option = (uint32_t) size;
  executing->Wait.count = submit_type;

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
#endif
}
