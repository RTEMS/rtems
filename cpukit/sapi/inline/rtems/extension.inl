/*  extension.inl
 *
 *  This file contains the static inline implementation of the inlined routines
 *  from the Extension Manager.
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

#ifndef __EXTENSION_MANAGER_inl
#define __EXTENSION_MANAGER_inl

/*PAGE
 *
 *  _Extension_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a extension control block from
 *  the inactive chain of free extension control blocks.
 */

RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Allocate( void )
{
  return (Extension_Control *) _Objects_Allocate( &_Extension_Information );
}

/*PAGE
 *
 *  _Extension_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a extension control block to the
 *  inactive chain of free extension control blocks.
 */

RTEMS_INLINE_ROUTINE void _Extension_Free (
  Extension_Control *the_extension
)
{
  _Objects_Free( &_Extension_Information, &the_extension->Object );
}

/*PAGE
 *
 *  _Extension_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps extension IDs to extension control blocks.
 *  If ID corresponds to a local extension, then it returns
 *  the extension control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */

RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Get (
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
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_extension is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Extension_Is_null (
  Extension_Control *the_extension
)
{
  return ( the_extension == NULL );
}

#endif
/* end of include file */
