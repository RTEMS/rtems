/*  rtems/posix/semaphore.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX Semaphores.
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
 
#ifndef __RTEMS_POSIX_SEMAPHORE_inl
#define __RTEMS_POSIX_SEMAPHORE_inl
 
/*PAGE
 *
 *  _POSIX_Semaphore_Allocate
 */
 
STATIC INLINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void )
{
  return (POSIX_Semaphore_Control *)
    _Objects_Allocate( &_POSIX_Semaphore_Information );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Free
 */
 
STATIC INLINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Get
 */
 
STATIC INLINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  return (POSIX_Semaphore_Control *)
    _Objects_Get( &_POSIX_Semaphore_Information, *id, location );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Is_null
 */
 
STATIC INLINE boolean _POSIX_Semaphore_Is_null (
  POSIX_Semaphore_Control *the_semaphore
)
{
  return !the_semaphore;
}

#endif
/*  end of include file */

