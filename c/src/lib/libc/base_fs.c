/*
 *  Base file system initialization
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

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

/*
 *  Global information for the base file system.
 */

rtems_user_env_t   rtems_global_user_env;
rtems_user_env_t * rtems_current_user_env = &rtems_global_user_env;

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
#if !defined(RTEMS_UNIX)
#if 0
  int                                   i;
#endif
  int                                   status;
  rtems_filesystem_mount_table_entry_t *entry;
  rtems_filesystem_mount_table_t       *mt;
  
  /*
   *  Set the default umask to "022".
   */

  rtems_filesystem_umask = S_IWOTH | S_IROTH;

  init_fs_mount_table();

  /*
   *  mount the first filesystem.
   */

  if ( rtems_filesystem_mount_table_size == 0 )
    rtems_fatal_error_occurred( 0xABCD0001 );

  mt = &rtems_filesystem_mount_table[0];

  status = mount(
     &entry, mt->fs_ops, mt->fsoptions, mt->device, mt->mount_point );

  if ( status == -1 )
    rtems_fatal_error_occurred( 0xABCD0002 );

  rtems_filesystem_link_counts = 0;
  rtems_filesystem_root        = entry->mt_fs_root;
  rtems_filesystem_current     = rtems_filesystem_root;

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

  /*
   *  This code if if'ed 0 out because you can't mount another
   *  filesystem properly until the mount point it will be 
   *  mounted onto is created.  Moreover, if it is going to 
   *  use a device, then it is REALLY unfair to attempt this
   *  before device drivers are initialized.
   */

#if 0
  /*
   *  Now if there are other filesystems to mount, go for it.
   */

  for ( i=1 ; i < rtems_filesystem_mount_table_size ; i++ ) {
    mt = &rtems_filesystem_mount_table[0];

    status = mount(
       &entry, mt->fs_ops, mt->fsoptions, mt->device, mt->mount_point );

    if ( status == -1 )
      rtems_fatal_error_occurred( 0xABCD0003 );
  }
#endif
#endif
}
