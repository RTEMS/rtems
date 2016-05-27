/**
 *  @file
 *
 *  @brief RTEMS Delete Message Queue
 *  @ingroup ClassicMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_message_queue_delete(
  rtems_id id
)
{
  Message_queue_Control *the_message_queue;
  Thread_queue_Context   queue_context;

  _Objects_Allocator_lock();
  the_message_queue = _Message_queue_Get( id, &queue_context );

  if ( the_message_queue == NULL ) {
    _Objects_Allocator_unlock();

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Message_queue_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _CORE_message_queue_Acquire_critical(
    &the_message_queue->message_queue,
    &queue_context
  );

  _Objects_Close( &_Message_queue_Information, &the_message_queue->Object );

  _Thread_queue_Context_set_MP_callout(
    &queue_context,
    _Message_queue_MP_Send_object_was_deleted
  );
  _CORE_message_queue_Close(
    &the_message_queue->message_queue,
    &queue_context
  );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( the_message_queue->attribute_set ) ) {
    _Objects_MP_Close(
      &_Message_queue_Information,
      the_message_queue->Object.id
    );

    _Message_queue_MP_Send_process_packet(
      MESSAGE_QUEUE_MP_ANNOUNCE_DELETE,
      the_message_queue->Object.id,
      0,                         /* Not used */
      0
    );
  }
#endif

  _Message_queue_Free( the_message_queue );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
