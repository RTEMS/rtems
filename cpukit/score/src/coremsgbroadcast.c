/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Broadcast().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/objectimpl.h>

Status_Control _CORE_message_queue_Broadcast(
  CORE_message_queue_Control *the_message_queue,
  const void                 *buffer,
  size_t                      size,
  uint32_t                   *count,
  Thread_queue_Context       *queue_context
)
{
  uint32_t number_broadcasted;

  if ( size > the_message_queue->maximum_message_size ) {
    _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
    return STATUS_MESSAGE_INVALID_SIZE;
  }

  number_broadcasted = 0;

  _CORE_message_queue_Acquire_critical( the_message_queue, queue_context );

  while (
    _CORE_message_queue_Dequeue_receiver(
      the_message_queue,
      buffer,
      size,
      0,
      queue_context
    ) != NULL
  ) {
    number_broadcasted += 1;

    _CORE_message_queue_Acquire( the_message_queue, queue_context );
  }

  _CORE_message_queue_Release( the_message_queue, queue_context );

  *count = number_broadcasted;
  return STATUS_SUCCESSFUL;
}
