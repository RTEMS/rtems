/*  rtems/posix/pthread.inl
 *
 *  This include file contains the macro implementation of the private 
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
 
#define _POSIX_Threads_Allocate() \
  (Thread_Control *) _Objects_Allocate( &_POSIX_Threads_Information )
 
/*PAGE
 *
 *  _POSIX_Threads_Free
 */
 
#define _POSIX_Threads_Free( _the_pthread ) \
  _Objects_Free( &_POSIX_Threads_Information, &(_the_pthread)->Object )
 
/*PAGE
 *
 *  _POSIX_Threads_Get
 */
 
#define _POSIX_Threads_Get( _id, _location ) \
  (Thread_Control *) \
    _Objects_Get( &_POSIX_Threads_Information, (Objects_Id)(_id), (_location) )
 
/*PAGE
 *
 *  _POSIX_Threads_Is_null
 */
 
#define _POSIX_Threads_Is_null( _the_pthread ) \
  (!(_the_pthread))

#endif
/*  end of include file */

