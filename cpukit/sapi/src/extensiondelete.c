/**
 * @file
 *
 * @ingroup ClassicUserExtensions
 *
 * @brief User Extensions Implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/extensionimpl.h>
#include <rtems/score/userextimpl.h>

rtems_status_code rtems_extension_delete(
  rtems_id id
)
{
  rtems_status_code  status;
  Extension_Control *the_extension;

  _Objects_Allocator_lock();

  the_extension = _Extension_Get( id );

  if ( the_extension != NULL ) {
    _Objects_Close( &_Extension_Information, &the_extension->Object );
    _User_extensions_Remove_set( &the_extension->Extension );
    _Extension_Free( the_extension );
    status = RTEMS_SUCCESSFUL;
  } else {
    status = RTEMS_INVALID_ID;
  }

  _Objects_Allocator_unlock();
  return status;
}
