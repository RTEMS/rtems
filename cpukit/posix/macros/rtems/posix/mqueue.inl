/*  rtems/posix/mqueue.inl
 *
 *  This include file contains the macro implementation of the private 
 *  inlined routines for POSIX Message Queue.
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
 
#ifndef __RTEMS_POSIX_MESSAGE_QUEUE_inl
#define __RTEMS_POSIX_MESSAGE_QUEUE_inl
 
/*PAGE
 *
 *  _POSIX_Message_queue_Allocate
 */
 
#define _POSIX_Message_queue_Allocate() \
  (POSIX_Message_queue_Control *) \
    _Objects_Allocate( &_POSIX_Message_queue_Information )
 
/*PAGE
 *
 *  _POSIX_Message_queue_Free
 */
 
#define _POSIX_Message_queue_Free( _the_mq ) \
  _Objects_Free( &_POSIX_Message_queue_Information, &(_the_mq)->Object )

/*PAGE
 *
 *  _POSIX_Message_queue_Namespace_remove
 */
 
#define _POSIX_Message_queue_Namespace_remove( _the_mq ) \
  _Objects_Namespace_remove( \
    &_POSIX_Message_queue_Information, &(_the_mq)->Object )
 
/*PAGE
 *
 *  _POSIX_Message_queue_Get
 */
 
#define _POSIX_Message_queue_Get( _id, _location ) \
  (POSIX_Message_queue_Control *) \
    _Objects_Get( &_POSIX_Message_queue_Information, (_id), (_location) )
 
/*PAGE
 *
 *  _POSIX_Message_queue_Is_null
 */
 
#define _POSIX_Message_queue_Is_null( _the_mq ) \
  (!(_the_mq))

/*PAGE
 *
 *  _POSIX_Message_queue_Priority_to_core
 */
 
#define _POSIX_Message_queue_Priority_to_core( _priority ) \
  ((_priority) * -1)

/*
 *  _POSIX_Message_queue_Priority_from_core
 */
 
/* absolute value without a library dependency */
#define _POSIX_Message_queue_Priority_from_core( _priority ) \
  ((((CORE_message_queue_Submit_types)(_priority)) >= 0) ? \
    (_priority) : \
    -((CORE_message_queue_Submit_types)(_priority)))

#endif
/*  end of include file */

