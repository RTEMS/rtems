/**
 * @file
 *
 * @brief Insert a Message into the Message Queue
 * @ingroup RTEMSScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2005.
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

#if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
static bool _CORE_message_queue_Order(
  const void       *left,
  const Chain_Node *right
)
{
   const int                               *left_priority;
   const CORE_message_queue_Buffer_control *right_message;

   left_priority = (const int *) left;
   right_message = (const CORE_message_queue_Buffer_control *) right;

   return *left_priority <
     _CORE_message_queue_Get_message_priority( right_message );
}
#endif

void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message,
  const void                        *content_source,
  size_t                             content_size,
  CORE_message_queue_Submit_types    submit_type
)
{
  Chain_Control *pending_messages;

  the_message->Contents.size = content_size;

  _CORE_message_queue_Copy_buffer(
    content_source,
    the_message->Contents.buffer,
    content_size
  );

#if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
  the_message->priority = submit_type;
#endif

  pending_messages = &the_message_queue->Pending_messages;
  ++the_message_queue->number_of_pending_messages;

  if ( submit_type == CORE_MESSAGE_QUEUE_SEND_REQUEST ) {
    _Chain_Append_unprotected( pending_messages, &the_message->Node );
#if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
  } else  if ( submit_type != CORE_MESSAGE_QUEUE_URGENT_REQUEST ) {
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
