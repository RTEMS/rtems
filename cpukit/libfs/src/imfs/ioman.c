/*
 *  This file emulates the old Classic RTEMS IO manager directives
 *  which register and lookup names using the in-memory filesystem.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>
#include <string.h>

#include <rtems/libio_.h>

/*
 *  rtems_io_register_name
 *
 *  This assumes that all registered devices are character devices.
 */

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

rtems_status_code rtems_io_lookup_name(
  const char           *name,
  rtems_driver_name_t  *device_info
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  struct stat st;
  int rv = stat( name, &st );

  if ( rv == 0 && S_ISCHR( st.st_mode ) ) {
    device_info->device_name = name;
    device_info->device_name_length = strlen( name );
    device_info->major = rtems_filesystem_dev_major_t( st.st_rdev );
    device_info->minor = rtems_filesystem_dev_minor_t( st.st_rdev );
  } else {
    sc = RTEMS_UNSATISFIED;
  }

  return sc;
}
