/*  rtems/posix/key.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX key's.
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
 
#ifndef __RTEMS_POSIX_KEY_inl
#define __RTEMS_POSIX_KEY_inl
 
/*PAGE
 *
 *  _POSIX_Keys_Allocate
 */
 
STATIC INLINE POSIX_Keys_Control *_POSIX_Keys_Allocate( void )
{
  return (POSIX_Keys_Control *) _Objects_Allocate( &_POSIX_Keys_Information );
}
 
/*PAGE
 *
 *  _POSIX_Keys_Free
 */
 
STATIC INLINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
)
{
  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}
 
/*PAGE
 *
 *  _POSIX_Keys_Get
 */
 
STATIC INLINE POSIX_Keys_Control *_POSIX_Keys_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (POSIX_Keys_Control *)
    _Objects_Get( &_POSIX_Keys_Information, id, location );
}
 
/*PAGE
 *
 *  _POSIX_Keys_Is_null
 */
 
STATIC INLINE boolean _POSIX_Keys_Is_null (
  POSIX_Keys_Control *the_key
)
{
  return !the_key;
}

#endif
/*  end of include file */

