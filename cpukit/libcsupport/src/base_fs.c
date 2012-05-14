/*
 *  Base file system initialization
 *
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

/*
 *  Default mode for created files.
 */


/*
 *  rtems_filesystem_initialize
 *
 *  Initialize the foundation of the file system.  This is specified
 *  by the structure rtems_filesystem_mount_table.  The usual
 *  configuration is a single instantiation of the IMFS or miniIMFS with
 *  a single "/dev" directory in it.
 */

void rtems_filesystem_initialize( void )
{
  int rv = 0;
  const rtems_filesystem_mount_configuration *root_config =
    &rtems_filesystem_root_configuration;

  rv = mount(
    root_config->source,
    root_config->target,
    root_config->filesystemtype,
    root_config->options,
    root_config->data
  );
  if ( rv != 0 )
    rtems_fatal_error_occurred( 0xABCD0002 );

  /*
   *  Traditionally RTEMS devices are under "/dev" so install this directory.
   *
   *  If the mkdir() fails, we can't print anything so just fatal error.
   *
   *  NOTE: UNIX root is 755 and owned by root/root (0/0).  It is actually
   *        created that way by the IMFS.
   */

  rv = mkdir( "/dev", 0777);
  if ( rv != 0 )
    rtems_fatal_error_occurred( 0xABCD0003 );

  /*
   *  You can't mount another filesystem properly until the mount point
   *  it will be mounted onto is created.  Moreover, if it is going to
   *  use a device, then it is REALLY unfair to attempt this
   *  before device drivers are initialized.  So we return via a base
   *  filesystem image and nothing auto-mounted at this point.
   */
}
