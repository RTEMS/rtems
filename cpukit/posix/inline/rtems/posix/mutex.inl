/*  rtems/posix/mutex.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX mutex's.
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
 *  _POSIX_Mutex_Get_support
 *
 *  NOTE: The support macro makes it possible for both to use exactly
 *        the same code to check for NULL id pointer and
 *        PTHREAD_MUTEX_INITIALIZER without adding overhead.
 */

#define ___POSIX_Mutex_Get_support( _id, _location ) \
  do { \
    int _status; \
    \
    if ( !_id ) { \
      *_location = OBJECTS_ERROR; \
      return (POSIX_Mutex_Control *) 0; \
    }  \
    \
    if ( *_id == PTHREAD_MUTEX_INITIALIZER ) { \
      /* \
       *  Do an "auto-create" here. \
       */ \
    \
      _status = pthread_mutex_init( _id, 0 ); \
      if ( _status ) { \
        *_location = OBJECTS_ERROR;  \
        return (POSIX_Mutex_Control *) 0; \
      } \
    } \
  } while (0)
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  ___POSIX_Mutex_Get_support( id, location );

  return (POSIX_Mutex_Control *)
    _Objects_Get( &_POSIX_Mutex_Information, *id, location );
}

RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get_interrupt_disable (
  Objects_Id        *id,
  Objects_Locations *location,
  ISR_Level         *level
)
{
  ___POSIX_Mutex_Get_support( id, location );

  return (POSIX_Mutex_Control *)
    _Objects_Get_isr_disable( &_POSIX_Mutex_Information, *id, location, level );
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

