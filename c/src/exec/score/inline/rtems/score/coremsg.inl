/*  coremsg.inl
 *
 *  This include file contains the static inline implementation of all
 *  inlined routines in the Core Message Handler.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
 */
 
STATIC INLINE CORE_message_queue_Status _CORE_message_queue_Send(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
    api_message_queue_mp_support,
    CORE_MESSAGE_QUEUE_SEND_REQUEST
  );
}
 
/*PAGE
 *
 *  _CORE_message_queue_Urgent
 *
 */
 
STATIC INLINE CORE_message_queue_Status _CORE_message_queue_Urgent(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support
)
{
  return _CORE_message_queue_Submit(
    the_message_queue,
    buffer,
    size,
    id,
    api_message_queue_mp_support,
    CORE_MESSAGE_QUEUE_URGENT_REQUEST
 );
}

/*PAGE
 *
 *  _CORE_message_queue_Copy_buffer
 *
 */

STATIC INLINE void _CORE_message_queue_Copy_buffer (
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
 */

STATIC INLINE CORE_message_queue_Buffer_control *
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
 */

STATIC INLINE void _CORE_message_queue_Free_message_buffer (
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
 */

STATIC INLINE
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
 */
 
STATIC INLINE boolean _CORE_message_queue_Is_priority(
  CORE_message_queue_Attributes *the_attribute
)
{
  return (the_attribute->discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY);
}

/*PAGE
 *
 *  _CORE_message_queue_Append
 *
 */

STATIC INLINE void _CORE_message_queue_Append (
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
 */

STATIC INLINE void _CORE_message_queue_Prepend (
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
 */

STATIC INLINE boolean _CORE_message_queue_Is_null (
  CORE_message_queue_Control *the_message_queue
)
{
  return ( the_message_queue == NULL  );
}

/*PAGE
 *
 *  _CORE_message_queue_Is_notify_enabled
 *
 */
 
STATIC INLINE boolean _CORE_message_queue_Is_notify_enabled (
  CORE_message_queue_Control *the_message_queue
)
{
  return (the_message_queue->notify_handler != NULL);
}
 
/*PAGE
 *
 *  _CORE_message_queue_Set_notify
 *
 */
 
STATIC INLINE void _CORE_message_queue_Set_notify (
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
