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
#include <rtems/rtems/support.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_extension_create(
  rtems_name                    name,
  const rtems_extensions_table *extension_table,
  rtems_id                     *id
)
{
  Extension_Control *the_extension;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  the_extension = _Extension_Allocate();

  if ( !the_extension ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  _User_extensions_Add_set_with_table( &the_extension->Extension, extension_table );

  _Objects_Open(
    &_Extension_Information,
    &the_extension->Object,
    (Objects_Name) name
  );

  *id = the_extension->Object.id;
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Extension_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Extension_Information);
}

RTEMS_SYSINIT_ITEM(
  _Extension_Manager_initialization,
  RTEMS_SYSINIT_USER_EXTENSIONS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
