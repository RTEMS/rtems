/*  coremsg.inl
 *
 *  This include file contains the macro implementation of all
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

/*PAGE
 *
 *  _CORE_message_queue_Send
 *
 */
 
#define _CORE_message_queue_Send( _the_message_queue, _buffer, _size, \
_id, _api_message_queue_mp_support ) \
  _CORE_message_queue_Submit( (_the_message_queue), (_buffer), (_size), \
     (_id), (_api_message_queue_mp_support), CORE_MESSAGE_QUEUE_SEND_REQUEST )
 
/*PAGE
 *
 *  _CORE_message_queue_Urgent
 *
 */
 
#define _CORE_message_queue_Urgent( _the_message_queue, _buffer, _size, \
_id, _api_message_queue_mp_support ) \
  _CORE_message_queue_Submit( (_the_message_queue), (_buffer), (_size), \
     (_id), (_api_message_queue_mp_support), CORE_MESSAGE_QUEUE_URGENT_REQUEST )

/*PAGE
 *
 *  _CORE_message_queue_Copy_buffer
 */

#define _CORE_message_queue_Copy_buffer( _source, _destination, _size ) \
  memcpy( _destination, _source, _size)

/*PAGE
 *
 *  _CORE_message_queue_Allocate_message_buffer
 *
 */

#define _CORE_message_queue_Allocate_message_buffer( _the_message_queue ) \
  (CORE_message_queue_Buffer_control *) \
    _Chain_Get( &(_the_message_queue)->Inactive_messages )

/*PAGE
 *
 *  _CORE_message_queue_Free_message_buffer
 *
 */

#define _CORE_message_queue_Free_message_buffer( _the_message_queue, _the_message ) \
  _Chain_Append( \
    &(_the_message_queue)->Inactive_messages, \
    &(_the_message)->Node \
  )

/*PAGE
 *
 *  _CORE_message_queue_Is_priority
 *
 */
 
#define _CORE_message_queue_Is_priority( _the_attribute ) \
  ((_the_attribute)->discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY)

/*PAGE
 *
 *  _CORE_message_queue_Get_pending_message
 *
 */

#define _CORE_message_queue_Get_pending_message( _the_message_queue ) \
   (CORE_message_queue_Buffer_control *) \
     _Chain_Get_unprotected( &(_the_message_queue)->Pending_messages )

/*PAGE
 *
 *  _CORE_message_queue_Append
 *
 */

#define _CORE_message_queue_Append( _the_message_queue, _the_message ) \
   _Chain_Append( &(_the_message_queue)->Pending_messages, \
                  &(_the_message)->Node )

/*PAGE
 *
 *  _CORE_message_queue_Prepend
 *
 */

#define _CORE_message_queue_Prepend( _the_message_queue, _the_message ) \
   _Chain_Prepend( &(_the_message_queue)->Pending_messages, \
                   &(_the_message)->Node )

/*PAGE
 *
 *  _CORE_message_queue_Is_null
 *
 */

#define _CORE_message_queue_Is_null( _the_message_queue ) \
   ( (_the_message_queue) == NULL  )

/*PAGE
 *
 *  _CORE_message_queue_Is_notify_enabled
 *
 */
 
#define _CORE_message_queue_Is_notify_enabled( _the_message_queue ) \
  ( (_the_message_queue)->notify_handler != NULL )
 
/*PAGE
 *
 *  _CORE_message_queue_Set_notify
 *
 */
 
#define _CORE_message_queue_Set_notify( \
  _the_message_queue, _the_handler, _the_argument ) \
    do { \
      (_the_message_queue->notify_handler  = (_the_handler); \
      (_the_message_queue->notify_argument = (_the_argument); \
    } while ( 0 )

#endif
/* end of include file */
