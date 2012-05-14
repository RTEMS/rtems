/**
 * @file rtems/posix/mutex.inl
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX mutex's.
 */

/*  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
#ifndef _RTEMS_POSIX_MUTEX_H
# error "Never use <rtems/posix/mutex.inl> directly; include <rtems/posix/mutex.h> instead."
#endif

#ifndef _RTEMS_POSIX_MUTEX_INL
#define _RTEMS_POSIX_MUTEX_INL
 
/*
 *  _POSIX_Mutex_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void )
{
  return (POSIX_Mutex_Control *) _Objects_Allocate( &_POSIX_Mutex_Information );
}
 
/*
 *  _POSIX_Mutex_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free (
  POSIX_Mutex_Control *the_mutex
)
{
  _Objects_Free( &_POSIX_Mutex_Information, &the_mutex->Object );
}
 
/*
 *  _POSIX_Mutex_Is_null
 */
 
RTEMS_INLINE_ROUTINE bool _POSIX_Mutex_Is_null (
  POSIX_Mutex_Control *the_mutex
)
{
  return !the_mutex;
}

#endif
/*  end of include file */

