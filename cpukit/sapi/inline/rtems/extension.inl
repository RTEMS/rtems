/**
 * @file
 *
 * @ingroup ClassicUserExtensions
 *
 * @brief User Extensions API.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __EXTENSION_MANAGER_inl
#define __EXTENSION_MANAGER_inl

RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Allocate( void )
{
  return (Extension_Control *) _Objects_Allocate( &_Extension_Information );
}

RTEMS_INLINE_ROUTINE void _Extension_Free (
  Extension_Control *the_extension
)
{
  _Objects_Free( &_Extension_Information, &the_extension->Object );
}

RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Extension_Control *)
    _Objects_Get( &_Extension_Information, id, location );
}

RTEMS_INLINE_ROUTINE bool _Extension_Is_null (
  Extension_Control *the_extension
)
{
  return ( the_extension == NULL );
}

#endif
/* end of include file */
