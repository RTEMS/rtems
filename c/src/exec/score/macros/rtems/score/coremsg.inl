/*  coremsg.inl
 *
 *  This include file contains the macro implementation of all
 *  inlined routines in the Core Message Handler.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
       _id, _api_message_queue_mp_support, _wait, _timeout ) \
  _CORE_message_queue_Submit( (_the_message_queue), (_buffer), (_size), \
     (_id), (_api_message_queue_mp_support), \
     CORE_MESSAGE_QUEUE_SEND_REQUEST, (_wait), (_timeout) )
 
/*PAGE
 *
 *  _CORE_message_queue_Urgent
 *
 */
 
#define _CORE_message_queue_Urgent( _the_message_queue, _buffer, _size, \
       _id, _api_message_queue_mp_support, _wait, _timeout ) \
  _CORE_message_queue_Submit( (_the_message_queue), (_buffer), (_size), \
     (_id), (_api_message_queue_mp_support), \
     CORE_MESSAGE_QUEUE_URGENT_REQUEST, (_wait), (_timeout) )

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
      (_the_message_queue)->notify_handler  = (_the_handler); \
      (_the_message_queue)->notify_argument = (_the_argument); \
    } while ( 0 )

#endif
/* end of include file */
