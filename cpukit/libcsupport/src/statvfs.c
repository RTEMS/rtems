/*
 *  COPYRIGHT (c) 2009 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
/*
 * The statvfs as defined by the SUS:
 *    http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/statvfs.h.html
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include <sys/statvfs.h>

/*
 *  Data structures and routines private to mount/unmount pair.
 */
extern rtems_chain_control rtems_filesystem_mount_table_control;

int
statvfs (const char *path, struct statvfs *sb)
{
  rtems_filesystem_location_info_t      loc;
  rtems_filesystem_location_info_t     *fs_mount_root;
  rtems_filesystem_mount_table_entry_t *mt_entry;
  int                                   result;

  /*
   *  Get
   *    The root node of the mounted filesytem.
   *    The node for the directory that the fileystem is mounted on.
   *    The mount entry that is being refered to.
   */

  if ( rtems_filesystem_evaluate_path( path, strlen( path ), 0x0, &loc, true ) )
    return -1;

  mt_entry      = loc.mt_entry;
  fs_mount_root = &mt_entry->mt_fs_root;

  if ( !fs_mount_root->ops->statvfs_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  memset (sb, 0, sizeof (struct statvfs));
  
  result = ( fs_mount_root->ops->statvfs_h )( fs_mount_root, sb );
  
  rtems_filesystem_freenode( &loc );

  return result;
}
