/**
 *  @file
 *
 *  @brief RTEMS Create Port
 *  @ingroup ClassicDPMEM
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/score/thread.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  uint32_t      length,
  rtems_id     *id
)
{
  Dual_ported_memory_Control *the_port;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  if ( !_Addresses_Is_aligned( internal_start ) ||
       !_Addresses_Is_aligned( external_start ) )
    return RTEMS_INVALID_ADDRESS;

  the_port = _Dual_ported_memory_Allocate();

  if ( !the_port ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  the_port->internal_base = internal_start;
  the_port->external_base = external_start;
  the_port->length        = length - 1;

  _Objects_Open(
    &_Dual_ported_memory_Information,
    &the_port->Object,
    (Objects_Name) name
  );

  *id = the_port->Object.id;
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Dual_ported_memory_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Dual_ported_memory_Information );
}

RTEMS_SYSINIT_ITEM(
  _Dual_ported_memory_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
