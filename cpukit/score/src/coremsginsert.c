/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Insert_message().
 */

/*
 *  COPYRIGHT (c) 1989-2005.
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

#if defined( RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY )
static bool _CORE_message_queue_Order(
  const void       *key,
  const Chain_Node *left,
  const Chain_Node *right
)
{
  const int                       *left_priority;
  const CORE_message_queue_Buffer *right_message;

  (void) left;
  left_priority = (const int *) key;
  right_message = (const CORE_message_queue_Buffer *) right;

  return *left_priority <
         _CORE_message_queue_Get_message_priority( right_message );
}
#endif

void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control     *the_message_queue,
  CORE_message_queue_Buffer      *the_message,
  const void                     *content_source,
  size_t                          content_size,
  CORE_message_queue_Submit_types submit_type
)
{
  Chain_Control *pending_messages;

  the_message->size = content_size;

  _CORE_message_queue_Copy_buffer(
    content_source,
    the_message->buffer,
    content_size
  );

#if defined( RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY )
  the_message->priority = submit_type;
#endif

  pending_messages = &the_message_queue->Pending_messages;
  ++the_message_queue->number_of_pending_messages;

  if ( submit_type == CORE_MESSAGE_QUEUE_SEND_REQUEST ) {
    _Chain_Append_unprotected( pending_messages, &the_message->Node );
#if defined( RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY )
  } else if ( submit_type != CORE_MESSAGE_QUEUE_URGENT_REQUEST ) {
    int priority;

    priority = _CORE_message_queue_Get_message_priority( the_message );
    _Chain_Insert_ordered_unprotected(
      pending_messages,
      &the_message->Node,
      &priority,
      _CORE_message_queue_Order
    );
#endif
  } else {
    _Chain_Prepend_unprotected( pending_messages, &the_message->Node );
  }
}
