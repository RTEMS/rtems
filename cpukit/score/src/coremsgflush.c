/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Flush().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

uint32_t   _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  Chain_Node *inactive_head;
  Chain_Node *inactive_first;
  Chain_Node *message_queue_first;
  Chain_Node *message_queue_last;
  uint32_t    count;

  /*
   *  Currently, RTEMS supports no API that has both flush and blocking
   *  sends.  Thus, this routine assumes that there are no senders
   *  blocked waiting to send messages.  In the event, that an API is
   *  added that can flush a message queue when threads are blocked
   *  waiting to send, there are two basic behaviors envisioned:
   *
   *  (1) The thread queue of pending senders is a logical extension
   *  of the pending message queue.  In this case, it should be
   *  flushed using the _Thread_queue_Flush_critical() service with a status
   *  such as CORE_MESSAGE_QUEUE_SENDER_FLUSHED (which currently does
   *  not exist).  This can be implemented without changing the "big-O"
   *  of the message flushing part of the routine.
   *
   *  (2) Only the actual messages queued should be purged.  In this case,
   *  the blocked sender threads must be allowed to send their messages.
   *  In this case, the implementation will be forced to individually
   *  dequeue the senders and queue their messages.  This will force
   *  this routine to have "big O(n)" where n is the number of blocked
   *  senders.  If there are more messages pending than senders blocked,
   *  then the existing flush code can be used to dispose of the remaining
   *  pending messages.
   *
   *  For now, though, we are very happy to have a small routine with
   *  fixed execution time that only deals with pending messages.
   */

  _CORE_message_queue_Acquire_critical( the_message_queue, queue_context );

  count = the_message_queue->number_of_pending_messages;
  if ( count != 0 ) {
    the_message_queue->number_of_pending_messages = 0;

    inactive_head = _Chain_Head( &the_message_queue->Inactive_messages );
    inactive_first = inactive_head->next;
    message_queue_first = _Chain_First( &the_message_queue->Pending_messages );
    message_queue_last = _Chain_Last( &the_message_queue->Pending_messages );

    inactive_head->next = message_queue_first;
    message_queue_last->next = inactive_first;
    inactive_first->previous = message_queue_last;
    message_queue_first->previous = inactive_head;

    _Chain_Initialize_empty( &the_message_queue->Pending_messages );
  }

  _CORE_message_queue_Release( the_message_queue, queue_context );
  return count;
}
