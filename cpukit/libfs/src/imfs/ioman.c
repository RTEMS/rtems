/**
 * @file
 *
 * @ingroup ClassicIO
 *
 * @brief RTEMS Register IO Name
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <string.h>

#include <rtems/libio_.h>

rtems_status_code rtems_io_register_name(
  const char                *device_name,
  rtems_device_major_number  major,
  rtems_device_minor_number  minor
)
{
  int    status;
  dev_t  dev;

  dev = rtems_filesystem_make_dev_t( major, minor );
  status = mknod( device_name, 0777 | S_IFCHR, dev );

  /* this is the only error returned by the old version */
  if ( status )
    return RTEMS_TOO_MANY;

  return RTEMS_SUCCESSFUL;
}
