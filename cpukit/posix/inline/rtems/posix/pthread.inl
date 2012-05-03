/**
 * @file rtems/posix/pthread.inl
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
#ifndef _RTEMS_POSIX_PTHREAD_H
# error "Never use <rtems/posix/pthread.inl> directly; include <rtems/posix/pthread.h> instead."
#endif

#ifndef _RTEMS_POSIX_PTHREAD_INL
#define _RTEMS_POSIX_PTHREAD_INL
 
/*
 *  _POSIX_Threads_Allocate
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_POSIX_Threads_Information );
}
 
/*
 *  _POSIX_Threads_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free (
  Thread_Control *the_pthread
)
{
  _Objects_Free( &_POSIX_Threads_Information, &the_pthread->Object );
}
 
/*
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
 
/*
 *  _POSIX_Threads_Is_null
 */
 
RTEMS_INLINE_ROUTINE bool _POSIX_Threads_Is_null (
  Thread_Control *the_pthread
)
{
  return !the_pthread;
}

#endif
/*  end of include file */

