/*  message.inl
 *
 *  This include file contains the macro implementation of all
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

/*PAGE
 *
 *  _Message_queue_Copy_buffer
 */

#define _Message_queue_Copy_buffer( _source, _destination ) \
  memcpy( _destination, _source, _size)

/*PAGE
 *
 *  _Message_queue_Allocate_message_buffer
 *
 */

#define _Message_queue_Allocate_message_buffer() \
   (Message_queue_Buffer_control *) \
     _Chain_Get( &_Message_queue_Inactive_messages )

/*PAGE
 *
 *  _Message_queue_Free_message_buffer
 *
 */

#define _Message_queue_Free_message_buffer( _the_message ) \
   _Chain_Append( &_Message_queue_Inactive_messages, &(_the_message)->Node )

/*PAGE
 *
 *  _Message_queue_Get_pending_message
 *
 */

#define _Message_queue_Get_pending_message( _the_message_queue ) \
   (Message_queue_Buffer_control *) \
     _Chain_Get_unprotected( &(_the_message_queue)->Pending_messages )

/*PAGE
 *
 *  _Message_queue_Append
 *
 */

#define _Message_queue_Append( _the_message_queue, _the_message ) \
   _Chain_Append( &(_the_message_queue)->Pending_messages, \
                  &(_the_message)->Node )

/*PAGE
 *
 *  _Message_queue_Prepend
 *
 */

#define _Message_queue_Prepend( _the_message_queue, _the_message ) \
   _Chain_Prepend( &(_the_message_queue)->Pending_messages, \
                   &(_the_message)->Node )

/*PAGE
 *
 *  _Message_queue_Is_null
 *
 */

#define _Message_queue_Is_null( _the_message_queue ) \
   ( (_the_message_queue) == NULL  )

/*PAGE
 *
 *  _Message_queue_Free
 *
 */

#define _Message_queue_Free( _the_message_queue ) \
  do { \
    \
    if ( (_the_messsage_queue)->message_buffers ) { \
      _Workspace_Free((void *) (_the_message_queue)->message_buffers); \
      (_the_message_queue)->message_buffers = 0; \
    }
    \
    _Objects_Free( \
      &_Message_queue_Information, \
      &(_the_message_queue)->Object \
    ); \
  } while ( 0 )


/*PAGE
 *
 *  _Message_queue_Get
 *
 */

#define _Message_queue_Get( _id, _location ) \
  (Message_queue_Control *) \
     _Objects_Get( &_Message_queue_Information, (_id), (_location) )

#endif
/* end of include file */
