/*  rtems/posix/semaphore.inl
 *
 *  This include file contains the macro implementation of the private 
 *  inlined routines for POSIX Semaphores.
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
 
#ifndef __RTEMS_POSIX_SEMAPHORE_inl
#define __RTEMS_POSIX_SEMAPHORE_inl
 
/*PAGE
 *
 *  _POSIX_Semaphore_Allocate
 */
 
#define _POSIX_Semaphore_Allocate() \
  (POSIX_Semaphore_Control *) \
    _Objects_Allocate( &_POSIX_Semaphore_Information );
 
/*PAGE
 *
 *  _POSIX_Semaphore_Free
 */
 
#define _POSIX_Semaphore_Free( _the_semaphore ) \
  _Objects_Free( &_POSIX_Semaphore_Information, &(_the_semaphore)->Object )
 
/*PAGE
 *
 *  _POSIX_Semaphore_Namespace_remove
 */
 
#define _POSIX_Semaphore_Namespace_remove( _the_semaphore ) \
  _Objects_Namespace_remove( \
    &_POSIX_Semaphore_Information, &(_the_semaphore)->Object )

/*PAGE
 *
 *  _POSIX_Semaphore_Get
 */
 
#define _POSIX_Semaphore_Get( _id, _location ) \
  (POSIX_Semaphore_Control *) \
    _Objects_Get( &_POSIX_Semaphore_Information, *(_id), (_location) )
 
/*PAGE
 *
 *  _POSIX_Semaphore_Is_null
 */
 
#define _POSIX_Semaphore_Is_null( _the_semaphore ) \
  (!(_the_semaphore))

#endif
/*  end of include file */

