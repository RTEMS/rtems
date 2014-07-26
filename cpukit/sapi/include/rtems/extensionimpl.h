/**
 * @file
 *
 * @ingroup ClassicUserExtensions
 *
 * @brief User Extensions API
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_EXTENSIONIMPL_H
#define _RTEMS_EXTENSIONIMPL_H

#include <rtems/extension.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SAPI_EXT_EXTERN
#define SAPI_EXT_EXTERN extern
#endif

SAPI_EXT_EXTERN Objects_Information  _Extension_Information;

/**
 *  @brief Initialize extension manager.
 *
 *  This routine initializes all extension manager related data structures.
 */
void _Extension_Manager_initialization(void);

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

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
