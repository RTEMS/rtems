/*  extension.inl
 *
 *  This file contains the static inline implementation of the inlined routines
 *  from the Extension Manager.
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

#ifndef __EXTENSION_MANAGER_inl
#define __EXTENSION_MANAGER_inl

/*PAGE
 *
 *  _Extension_Allocate
 *
 */

STATIC INLINE Extension_Control *_Extension_Allocate( void )
{
  return (Extension_Control *) _Objects_Allocate( &_Extension_Information );
}

/*PAGE
 *
 *  _Extension_Free
 *
 */

STATIC INLINE void _Extension_Free (
  Extension_Control *the_extension
)
{
  _Objects_Free( &_Extension_Information, &the_extension->Object );
}

/*PAGE
 *
 *  _Extension_Get
 *
 */

STATIC INLINE Extension_Control *_Extension_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Extension_Control *)
    _Objects_Get( &_Extension_Information, id, location );
}

/*PAGE
 *
 *  _Extension_Is_null
 *
 */

STATIC INLINE boolean _Extension_Is_null (
  Extension_Control *the_extension
)
{
  return ( the_extension == NULL );
}

#endif
/* end of include file */
