/*
 *  This file emulates the old Classic RTEMS IO manager directives
 *  which register and lookup names using the in-memory filesystem.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

#include <assert.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include "imfs.h"

#if defined(__linux__)
#define S_IFCHR __S_IFCHR
#endif

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
#if !defined(RTEMS_UNIX)
  int    status;
  dev_t  dev;

  dev = rtems_filesystem_make_dev_t( major, minor );
  status = mknod( device_name, 0777 | S_IFCHR, dev );

  /* this is the only error returned by the old version */
  if ( status )
    return RTEMS_TOO_MANY;
 
#endif
  return RTEMS_SUCCESSFUL;
}

/* 
 *  rtems_io_lookup_name
 *
 *  This version is not reentrant.
 *
 *  XXX - This is dependent upon IMFS and should not be.  
 *        Suggest adding a filesystem routine to fill in the device_info.
 */

rtems_status_code rtems_io_lookup_name(
  const char           *name,
  rtems_driver_name_t **device_info
)
{
#if !defined(RTEMS_UNIX)
  IMFS_jnode_t                      *the_jnode;
  rtems_filesystem_location_info_t   loc;
  static rtems_driver_name_t         device;
  int                                result;
  rtems_filesystem_node_types_t      node_type;

  result = rtems_filesystem_evaluate_path( name, 0x00, &loc, TRUE );
  the_jnode = loc.node_access;

  if ( !loc.ops->node_type_h ) {
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  node_type = (*loc.ops->node_type_h)( &loc );

  if ( (result != 0) || node_type != RTEMS_FILESYSTEM_DEVICE ) {
    *device_info = 0;
    rtems_filesystem_freenode( &loc );
    return RTEMS_UNSATISFIED;
  }

  device.device_name        = (char *) name;
  device.device_name_length = strlen( name );
  device.major              = the_jnode->info.device.major;
  device.minor              = the_jnode->info.device.minor;
  *device_info              = &device;

  rtems_filesystem_freenode( &loc );
   
#endif
  return RTEMS_SUCCESSFUL;
}
