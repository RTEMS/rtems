/**
 * @file
 *
 * @ingroup ClassicUserExtensionsImpl
 *
 * @brief Classic User Extensions Implementation
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

#include <rtems/extensiondata.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicUserExtensionsImpl User Extensions Implementation
 *
 * @ingroup RTEMSInternalClassic
 *
 * @{
 */

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

RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Get( Objects_Id id )
{
  return (Extension_Control *)
    _Objects_Get_no_protection( id, &_Extension_Information );
}

/** @} */ 

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
