/*  message.inl
 *
 *  This include file contains the static inline implementation of all
 *  inlined routines in the Message Manager.
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

#ifndef __MESSAGE_QUEUE_inl
#define __MESSAGE_QUEUE_inl

#include <rtems/wkspace.h>

/*PAGE
 *
 *  _Message_queue_Copy_buffer
 *
 */

STATIC INLINE void _Message_queue_Copy_buffer (
  void      *source,
  void      *destination,
  unsigned32 size
)
{
  memcpy(destination, source, size);
}

/*PAGE
 *
 *  _Message_queue_Allocate_message_buffer
 *
 */

STATIC INLINE Message_queue_Buffer_control *
_Message_queue_Allocate_message_buffer (
    Message_queue_Control *the_message_queue
)
{
   return (Message_queue_Buffer_control *)
     _Chain_Get( &the_message_queue->Inactive_messages );
}

/*PAGE
 *
 *  _Message_queue_Free_message_buffer
 *
 */

STATIC INLINE void _Message_queue_Free_message_buffer (
    Message_queue_Control        *the_message_queue,
    Message_queue_Buffer_control *the_message
)
{
  _Chain_Append( &the_message_queue->Inactive_messages, &the_message->Node );
}

/*PAGE
 *
 *  _Message_queue_Get_pending_message
 *
 */

STATIC INLINE
  Message_queue_Buffer_control *_Message_queue_Get_pending_message (
  Message_queue_Control *the_message_queue
)
{
  return (Message_queue_Buffer_control *)
    _Chain_Get_unprotected( &the_message_queue->Pending_messages );
}

/*PAGE
 *
 *  _Message_queue_Append
 *
 */

STATIC INLINE void _Message_queue_Append (
  Message_queue_Control        *the_message_queue,
  Message_queue_Buffer_control *the_message
)
{
  _Chain_Append( &the_message_queue->Pending_messages, &the_message->Node );
}

/*PAGE
 *
 *  _Message_queue_Prepend
 *
 */

STATIC INLINE void _Message_queue_Prepend (
  Message_queue_Control        *the_message_queue,
  Message_queue_Buffer_control *the_message
)
{
  _Chain_Prepend(
    &the_message_queue->Pending_messages,
    &the_message->Node
  );
}

/*PAGE
 *
 *  _Message_queue_Is_null
 *
 */

STATIC INLINE boolean _Message_queue_Is_null (
  Message_queue_Control *the_message_queue
)
{
  return ( the_message_queue == NULL  );
}


/*PAGE
 *
 *  _Message_queue_Free
 *
 */

STATIC INLINE void _Message_queue_Free (
  Message_queue_Control *the_message_queue
)
{
  if (the_message_queue->message_buffers)
  {
      _Workspace_Free((void *) the_message_queue->message_buffers);
      the_message_queue->message_buffers = 0;
  }
  
  _Objects_Free( &_Message_queue_Information, &the_message_queue->Object );
}

/*PAGE
 *
 *  _Message_queue_Get
 *
 */

STATIC INLINE Message_queue_Control *_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Message_queue_Control *)
     _Objects_Get( &_Message_queue_Information, id, location );
}

#endif
/* end of include file */
