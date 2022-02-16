/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Initialize().
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
#include <rtems/score/assert.h>

#define MESSAGE_SIZE_LIMIT \
  ( SIZE_MAX - sizeof( uintptr_t ) + 1 - sizeof( CORE_message_queue_Buffer ) )

RTEMS_STATIC_ASSERT(
  offsetof( CORE_message_queue_Buffer, buffer )
    == sizeof( CORE_message_queue_Buffer ),
  CORE_MESSAGE_QUEUE_BUFFER_OFFSET
);

Status_Control _CORE_message_queue_Initialize(
  CORE_message_queue_Control          *the_message_queue,
  CORE_message_queue_Disciplines       discipline,
  uint32_t                             maximum_pending_messages,
  size_t                               maximum_message_size,
  CORE_message_queue_Allocate_buffers  allocate_buffers,
  const void                          *arg
)
{
  size_t buffer_size;

  /* Make sure the message size computation does not overflow */
  if ( maximum_message_size > MESSAGE_SIZE_LIMIT ) {
    return STATUS_MESSAGE_QUEUE_INVALID_SIZE;
  }

  buffer_size = RTEMS_ALIGN_UP( maximum_message_size, sizeof( uintptr_t ) );
  _Assert( buffer_size >= maximum_message_size );

  buffer_size += sizeof( CORE_message_queue_Buffer );
  _Assert( buffer_size >= sizeof( CORE_message_queue_Buffer ) );

  /* Make sure the memory allocation size computation does not overflow */
  if ( maximum_pending_messages > SIZE_MAX / buffer_size ) {
    return STATUS_MESSAGE_QUEUE_INVALID_NUMBER;
  }

  the_message_queue->message_buffers = ( *allocate_buffers )(
    the_message_queue,
    (size_t) maximum_pending_messages * buffer_size,
    arg
  );

  if ( the_message_queue->message_buffers == NULL ) {
    return STATUS_MESSAGE_QUEUE_NO_MEMORY;
  }

  the_message_queue->maximum_pending_messages   = maximum_pending_messages;
  the_message_queue->number_of_pending_messages = 0;
  the_message_queue->maximum_message_size       = maximum_message_size;

  _CORE_message_queue_Set_notify( the_message_queue, NULL );
  _Chain_Initialize_empty( &the_message_queue->Pending_messages );
  _Thread_queue_Object_initialize( &the_message_queue->Wait_queue );

  if ( discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY ) {
    the_message_queue->operations = &_Thread_queue_Operations_priority;
  } else {
    the_message_queue->operations = &_Thread_queue_Operations_FIFO;
  }

  _Chain_Initialize (
    &the_message_queue->Inactive_messages,
    the_message_queue->message_buffers,
    (size_t) maximum_pending_messages,
    buffer_size
  );

  return STATUS_SUCCESSFUL;
}
