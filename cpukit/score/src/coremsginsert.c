/**
 * @file
 *
 * @brief Insert a Message into the Message Queue
 * @ingroup ScoreMessageQueue
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
  const Chain_Node *left,
  const Chain_Node *right
)
{
   const CORE_message_queue_Buffer_control *left_message;
   const CORE_message_queue_Buffer_control *right_message;

   left_message = (const CORE_message_queue_Buffer_control *) left;
   right_message = (const CORE_message_queue_Buffer_control *) right;

   return _CORE_message_queue_Get_message_priority( left_message ) <
     _CORE_message_queue_Get_message_priority( right_message );
}
#endif

void _CORE_message_queue_Insert_message(
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message,
  CORE_message_queue_Submit_types    submit_type
)
{
  Chain_Control *pending_messages;
#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  bool           notify;
#endif

  _CORE_message_queue_Set_message_priority( the_message, submit_type );
  pending_messages = &the_message_queue->Pending_messages;

#if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
  notify = ( the_message_queue->number_of_pending_messages == 0 );
#endif
  ++the_message_queue->number_of_pending_messages;

  if ( submit_type == CORE_MESSAGE_QUEUE_SEND_REQUEST ) {
    _Chain_Append_unprotected( pending_messages, &the_message->Node );
#if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
  } else  if ( submit_type != CORE_MESSAGE_QUEUE_URGENT_REQUEST ) {
    _Chain_Insert_ordered_unprotected(
      pending_messages,
      &the_message->Node,
      _CORE_message_queue_Order
    );
#endif
  } else {
    _Chain_Prepend_unprotected( pending_messages, &the_message->Node );
  }

  #if defined(RTEMS_SCORE_COREMSG_ENABLE_NOTIFICATION)
    /*
     *  According to POSIX, does this happen before or after the message
     *  is actually enqueued.  It is logical to think afterwards, because
     *  the message is actually in the queue at this point.
     */
    if ( notify && the_message_queue->notify_handler )
      (*the_message_queue->notify_handler)(the_message_queue->notify_argument);
  #endif
}
