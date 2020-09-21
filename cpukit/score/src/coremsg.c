/**
 *  @file
 *
 *  @brief Initialize a Message Queue
 *  @ingroup RTEMSScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/wkspace.h>

#define MESSAGE_SIZE_LIMIT \
  ( SIZE_MAX - sizeof( uintptr_t ) + 1 \
    - sizeof( CORE_message_queue_Buffer_control ) )

bool _CORE_message_queue_Initialize(
  CORE_message_queue_Control     *the_message_queue,
  CORE_message_queue_Disciplines  discipline,
  uint32_t                        maximum_pending_messages,
  size_t                          maximum_message_size
)
{
  size_t buffer_size;

  the_message_queue->maximum_pending_messages   = maximum_pending_messages;
  the_message_queue->number_of_pending_messages = 0;
  the_message_queue->maximum_message_size       = maximum_message_size;
  _CORE_message_queue_Set_notify( the_message_queue, NULL );

  /* Make sure the message size computation does not overflow */
  if ( maximum_message_size > MESSAGE_SIZE_LIMIT ) {
    return false;
  }

  buffer_size = RTEMS_ALIGN_UP( maximum_message_size, sizeof( uintptr_t ) );
  _Assert( buffer_size >= maximum_message_size );

  buffer_size += sizeof( CORE_message_queue_Buffer_control );
  _Assert( buffer_size >= sizeof( CORE_message_queue_Buffer_control ) );

  /* Make sure the memory allocation size computation does not overflow */
  if ( maximum_pending_messages > SIZE_MAX / buffer_size ) {
    return false;
  }

  the_message_queue->message_buffers = _Workspace_Allocate(
    (size_t) maximum_pending_messages * buffer_size
  );

  if ( the_message_queue->message_buffers == NULL ) {
    return false;
  }

  /*
   *  Initialize the pool of inactive messages, pending messages,
   *  and set of waiting threads.
   */
  _Chain_Initialize (
    &the_message_queue->Inactive_messages,
    the_message_queue->message_buffers,
    (size_t) maximum_pending_messages,
    buffer_size
  );

  _Chain_Initialize_empty( &the_message_queue->Pending_messages );

  _Thread_queue_Object_initialize( &the_message_queue->Wait_queue );

  if ( discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY ) {
    the_message_queue->operations = &_Thread_queue_Operations_priority;
  } else {
    the_message_queue->operations = &_Thread_queue_Operations_FIFO;
  }

  return true;
}
