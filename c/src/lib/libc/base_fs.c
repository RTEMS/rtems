/*
 *  Base file system initialization
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

#include <rtems.h>
#include <rtems/libio.h>
#include "imfs.h"
#include "libio_.h"

/*
 *  Global information for the base file system.
 */

rtems_filesystem_location_info_t rtems_filesystem_current;
rtems_filesystem_location_info_t rtems_filesystem_root;
nlink_t                          rtems_filesystem_link_counts;

/*
 *  Default mode for created files.
 */

mode_t rtems_filesystem_umask;

/*
 *  rtems_filesystem_initialize
 *
 *  Initialize the foundation of the file system with one instantiation
 *  of the IMFS with a single "/dev" directory in it.
 */

void rtems_filesystem_initialize( void )
{
#if !defined(RTEMS_UNIX)
  int                                   status;
  rtems_filesystem_mount_table_entry_t *first_entry;

  /*
   *  Set the default umask to "022".
   */

  rtems_filesystem_umask = S_IWOTH | S_IROTH;

  init_fs_mount_table();

  status = mount( 
     &first_entry,
     &IMFS_ops,
     "RW",
     NULL,
     NULL );
  if( status == -1 ){
    rtems_fatal_error_occurred( 0xABCD0002 );
  }

  rtems_filesystem_link_counts = 0;

  rtems_filesystem_root    = first_entry->mt_fs_root;

  rtems_filesystem_current = rtems_filesystem_root;

  /*
   *  Traditionally RTEMS devices are under "/dev" so install this directory.
   *
   *  If the mkdir() fails, we can't print anything so just fatal error.
   *
   *  NOTE: UNIX root is 755 and owned by root/root (0/0).
   */

  status = mkdir( "/dev", S_IRWXU | S_IRWXG | S_IRWXO );
  if ( status != 0 )
    rtems_fatal_error_occurred( 0xABCD0003 );
#endif
}
