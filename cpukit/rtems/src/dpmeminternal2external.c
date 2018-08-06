/**
 *  @file
 *
 *  @brief RTEMS Port Internal to External
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/dpmemimpl.h>
#include <rtems/score/address.h>

rtems_status_code rtems_port_internal_to_external(
  rtems_id   id,
  void      *internal,
  void     **external
)
{
  Dual_ported_memory_Control *the_port;
  ISR_lock_Context            lock_context;
  uintptr_t                   length;

  if ( external == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_port = _Dual_ported_memory_Get( id, &lock_context );

  if ( the_port == NULL ) {
    return RTEMS_INVALID_ID;
  }

  length = (uintptr_t) _Addresses_Subtract( internal, the_port->internal_base );

  if ( length > the_port->length ) {
    *external = internal;
  } else {
    *external = _Addresses_Add_offset( the_port->external_base, length );
  }

  _ISR_lock_ISR_enable( &lock_context );
  return RTEMS_SUCCESSFUL;
}
