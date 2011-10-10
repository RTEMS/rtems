/*
 *  This file emulates the old Classic RTEMS IO manager directives
 *  which register and lookup names using the in-memory filesystem.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include "imfs.h"

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

/*
 *  rtems_io_lookup_name
 *
 *  This version is reentrant.
 *
 *  XXX - This is dependent upon IMFS and should not be.
 *        Suggest adding a filesystem routine to fill in the device_info.
 */

rtems_status_code rtems_io_lookup_name(
  const char           *name,
  rtems_driver_name_t  *device_info
)
{
  IMFS_jnode_t                      *the_jnode;
  rtems_filesystem_location_info_t   loc;
  int                                result;
  rtems_filesystem_node_types_t      node_type;

  result = rtems_filesystem_evaluate_path(
      name, strlen( name ), 0x00, &loc, true );
  the_jnode = loc.node_access;

  node_type = (*loc.ops->node_type_h)( &loc );

  if ( (result != 0) || node_type != RTEMS_FILESYSTEM_DEVICE ) {
    rtems_filesystem_freenode( &loc );
    return RTEMS_UNSATISFIED;
  }

  device_info->device_name        = name;
  device_info->device_name_length = strlen( name );
  device_info->major              = the_jnode->info.device.major;
  device_info->minor              = the_jnode->info.device.minor;

  rtems_filesystem_freenode( &loc );

  return RTEMS_SUCCESSFUL;
}
