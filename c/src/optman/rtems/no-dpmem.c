/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/core/address.h>
#include <rtems/core/object.h>
#include <rtems/core/thread.h>

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
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_delete(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_internal_to_external(
  Objects_Id   id,
  void        *internal,
  void       **external
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_port_external_to_internal(
  Objects_Id   id,
  void        *external,
  void       **internal
)
{
  return RTEMS_NOT_CONFIGURED;
}
