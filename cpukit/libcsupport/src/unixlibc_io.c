/*
 *  UNIX Port C Library Support -- IO Manager Stubs
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

rtems_status_code rtems_io_register_name(
  const char                *device_name,
  rtems_device_major_number  major,
  rtems_device_minor_number  minor
)
{
  return 0; /* not supported */
}

rtems_status_code rtems_io_lookup_name(
  const char           *name,
  rtems_driver_name_t  *device_info
)
{
  return 0; /* not supported */
}
