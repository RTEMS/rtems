/*  rtems/posix/mqueue.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX Message Queue.
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
 
#ifndef __RTEMS_POSIX_MESSAGE_QUEUE_inl
#define __RTEMS_POSIX_MESSAGE_QUEUE_inl
 
/*PAGE
 *
 *  _POSIX_Message_queue_Allocate
 */
 
STATIC INLINE POSIX_Message_queue_Control *_POSIX_Message_queue_Allocate( void )
{
  return (POSIX_Message_queue_Control *)
    _Objects_Allocate( &_POSIX_Message_queue_Information );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Free
 */
 
STATIC INLINE void _POSIX_Message_queue_Free (
  POSIX_Message_queue_Control *the_mq
)
{
  _Objects_Free( &_POSIX_Message_queue_Information, &the_mq->Object );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Get
 */
 
STATIC INLINE POSIX_Message_queue_Control *_POSIX_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (POSIX_Message_queue_Control *)
    _Objects_Get( &_POSIX_Message_queue_Information, id, location );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Is_null
 */
 
STATIC INLINE boolean _POSIX_Message_queue_Is_null (
  POSIX_Message_queue_Control *the_mq
)
{
  return !the_mq;
}

/*PAGE
 *
 *  _POSIX_Message_queue_Priority_to_core
 */
 
STATIC INLINE Priority_Control _POSIX_Message_queue_Priority_to_core(
  unsigned int priority
)
{
  return priority;
}

#endif
/*  end of include file */

