/*  coremsg.inl
 *
 *  This include file contains the static inline implementation of all
 *  inlined routines in the Core Message Handler.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __CORE_MESSAGE_QUEUE_inl
#define __CORE_MESSAGE_QUEUE_inl

#include <string.h>   /* needed for memcpy */
 
/*PAGE
 *
 *  _CORE_message_queue_Send
 *
 *  DESCRIPTION:
 *
 *  This routine sends a message to the end of the specified message queue.
 */
 
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Send(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  boolean                                    wait,
  Watchdog_Interval                          timeout
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
#if defined(RTEMS_MULTIPROCESSING)
    api_message_queue_mp_support,
#else
    NULL,
#endif
    CORE_MESSAGE_QUEUE_SEND_REQUEST,
    wait,     /* sender may block */
    timeout   /* timeout interval */
  );
}
 
/*PAGE
 *
 *  _CORE_message_queue_Urgent
 *
 *  DESCRIPTION:
 *
 *  This routine sends a message to the front of the specified message queue.
 */
 
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Urgent(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  boolean                                    wait,
  Watchdog_Interval                          timeout
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
#if defined(RTEMS_MULTIPROCESSING)
    api_message_queue_mp_support,
#else
    NULL,
#endif
    CORE_MESSAGE_QUEUE_URGENT_REQUEST,
    wait,     /* sender may block */
    timeout   /* timeout interval */
 );
}

/*PAGE
 *
 *  _CORE_message_queue_Copy_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine copies the contents of the source message buffer
 *  to the destination message buffer.
 */

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Copy_buffer (
  void      *source,
  void      *destination,
  unsigned32 size
)
{
  memcpy(destination, source, size);
}

/*PAGE
 *
 *  _CORE_message_queue_Allocate_message_buffer
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message buffer from the inactive
 *  message buffer chain.
 */

RTEMS_INLINE_ROUTINE CORE_message_queue_Buffer_control *
_CORE_message_queue_Allocate_message_buffer (
    CORE_message_queue_Control *the_message_queue
)
{
   return (CORE_message_queue_Buffer_control *)
     _Chain_Get( &the_message_queue->Inactive_messages );
}

/*PAGE
 *
 *  _CORE_message_queue_Free_message_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine frees a message buffer to the inactive
 *  message buffer chain.
 */

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Free_message_buffer (
    CORE_message_queue_Control        *the_message_queue,
    CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append( &the_message_queue->Inactive_messages, &the_message->Node );
}

/*PAGE
 *
 *  _CORE_message_queue_Get_pending_message
 *
 *  DESCRIPTION:
 *
 *  This function removes the first message from the_message_queue
 *  and returns a pointer to it.
 */

RTEMS_INLINE_ROUTINE
  CORE_message_queue_Buffer_control *_CORE_message_queue_Get_pending_message (
  CORE_message_queue_Control *the_message_queue
)
{
  return (CORE_message_queue_Buffer_control *)
    _Chain_Get_unprotected( &the_message_queue->Pending_messages );
}

/*PAGE
 *
 *  _CORE_message_queue_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_message_queue_Is_priority(
  CORE_message_queue_Attributes *the_attribute
)
{
  return (the_attribute->discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY);
}

/*PAGE
 *
 *  _CORE_message_queue_Append
 *
 *  DESCRIPTION:
 *
 *  This routine places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 */

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Append (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Append( &the_message_queue->Pending_messages, &the_message->Node );
}

/*PAGE
 *
 *  _CORE_message_queue_Prepend
 *
 *  DESCRIPTION:
 *
 *  This routine places the_message at the front of the outstanding
 *  messages on the_message_queue.
 */

RTEMS_INLINE_ROUTINE void _CORE_message_queue_Prepend (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Buffer_control *the_message
)
{
  _Chain_Prepend(
    &the_message_queue->Pending_messages,
    &the_message->Node
  );
}

/*PAGE
 *
 *  _CORE_message_queue_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_message_queue is TRUE and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _CORE_message_queue_Is_null (
  CORE_message_queue_Control *the_message_queue
)
{
  return ( the_message_queue == NULL  );
}

/*PAGE
 *
 *  _CORE_message_queue_Is_notify_enabled
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if notification is enabled on this message
 *  queue and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_message_queue_Is_notify_enabled (
  CORE_message_queue_Control *the_message_queue
)
{
  return (the_message_queue->notify_handler != NULL);
}
 
/*PAGE
 *
 *  _CORE_message_queue_Set_notify
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the notification information for the_message_queue.
 */
 
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Set_notify (
  CORE_message_queue_Control        *the_message_queue,
  CORE_message_queue_Notify_Handler  the_handler,
  void                              *the_argument
)
{
  the_message_queue->notify_handler  = the_handler;
  the_message_queue->notify_argument = the_argument;
}

#endif
/* end of include file */
