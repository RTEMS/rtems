/*
 *  Dual Port Memory Manager
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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/dpmem.h>

void _Dual_ported_memory_Manager_initialization(
  unsigned32 maximum_ports
)
{
}

rtems_status_code rtems_port_create(
  rtems_name    name,
  void         *internal_start,
  void         *external_start,
  unsigned32    length,
  Objects_Id   *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_delete(
  Objects_Id id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_internal_to_external(
  Objects_Id   id,
  void        *internal,
  void       **external
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_external_to_internal(
  Objects_Id   id,
  void        *external,
  void       **internal
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
