/*
 *  This file emulates the old Classic RTEMS IO manager directives
 *  which register and lookup names using the in-memory filesystem.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <assert.h>

#include <rtems.h>
#include "libio_.h"
#include "imfs.h"

/* 
 *  rtems_io_register_name
 *
 *  This assumes that all registered devices are character devices.
 */

rtems_status_code rtems_io_register_name(
  char *device_name,
  rtems_device_major_number major,
  rtems_device_minor_number minor
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

/* 
 *  rtems_io_lookup_name
 *
 *  This version is not reentrant.
 */

rtems_status_code rtems_io_lookup_name(
  const char           *name,
  rtems_driver_name_t **device_info
)
{
  IMFS_jnode_t                      *the_jnode;
  rtems_filesystem_location_info_t   temp_loc;
  static rtems_driver_name_t         device;
  int                                result;

  result = rtems_filesystem_evaluate_path( name, 0x00, &temp_loc, TRUE );
  the_jnode = temp_loc.node_access;

  if ( (result != 0) || the_jnode->type != IMFS_DEVICE ) {
    *device_info = 0;
    return RTEMS_UNSATISFIED;
  }

  device.device_name        = (char *) name;
  device.device_name_length = strlen( name );
  device.major              = the_jnode->info.device.major;
  device.minor              = the_jnode->info.device.minor;
  *device_info              = &device;
  return RTEMS_SUCCESSFUL;

}
