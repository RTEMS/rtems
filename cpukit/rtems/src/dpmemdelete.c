/**
 *  @file
 *
 *  @brief RTEMS Delete Port
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

rtems_status_code rtems_port_delete(
  rtems_id id
)
{
  Dual_ported_memory_Control *the_port;
  ISR_lock_Context            lock_context;

  _Objects_Allocator_lock();
  the_port = _Dual_ported_memory_Get( id, &lock_context );

  if ( the_port == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_INVALID_ID;
  }

  _Objects_Close( &_Dual_ported_memory_Information, &the_port->Object );
  _ISR_lock_ISR_enable( &lock_context );
  _Dual_ported_memory_Free( the_port );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
