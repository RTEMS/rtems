/*  rtems/posix/pthread.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX threads.
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
 
#ifndef __RTEMS_POSIX_THREADS_inl
#define __RTEMS_POSIX_THREADS_inl
 
/*PAGE
 *
 *  _POSIX_Threads_Allocate
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_POSIX_Threads_Information );
}
 
/*PAGE
 *
 *  _POSIX_Threads_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free (
  Thread_Control *the_pthread
)
{
  _Objects_Free( &_POSIX_Threads_Information, &the_pthread->Object );
}
 
/*PAGE
 *
 *  _POSIX_Threads_Get
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Get (
  pthread_t          id,
  Objects_Locations *location
)
{
  return (Thread_Control *)
    _Objects_Get( &_POSIX_Threads_Information, (Objects_Id)id, location );
}
 
/*PAGE
 *
 *  _POSIX_Threads_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Threads_Is_null (
  Thread_Control *the_pthread
)
{
  return !the_pthread;
}

#endif
/*  end of include file */

