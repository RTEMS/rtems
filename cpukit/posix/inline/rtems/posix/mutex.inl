/*  rtems/posix/mutex.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX mutex's.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_MUTEX_inl
#define __RTEMS_POSIX_MUTEX_inl
 
/*PAGE
 *
 *  _POSIX_Mutex_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void )
{
  return (POSIX_Mutex_Control *) _Objects_Allocate( &_POSIX_Mutex_Information );
}
 
/*PAGE
 *
 *  _POSIX_Mutex_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free (
  POSIX_Mutex_Control *the_mutex
)
{
  _Objects_Free( &_POSIX_Mutex_Information, &the_mutex->Object );
}
 
/*PAGE
 *
 *  _POSIX_Mutex_Get
 */
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  int status;

  if ( !id ) {
    *location = OBJECTS_ERROR;
    return (POSIX_Mutex_Control *) 0;
  }

  if ( *id == PTHREAD_MUTEX_INITIALIZER ) {
    /*
     *  Do an "auto-create" here.
     */

    status = pthread_mutex_init( id, 0 );
    if ( status ) {
      *location = OBJECTS_ERROR;
      return (POSIX_Mutex_Control *) 0;
    }
  }

  /*
   *  Now call Objects_Get()
   */

  return (POSIX_Mutex_Control *)
    _Objects_Get( &_POSIX_Mutex_Information, *id, location );
}
 
/*PAGE
 *
 *  _POSIX_Mutex_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Mutex_Is_null (
  POSIX_Mutex_Control *the_mutex
)
{
  return !the_mutex;
}

#endif
/*  end of include file */

