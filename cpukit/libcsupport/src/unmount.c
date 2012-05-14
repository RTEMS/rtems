/*
 *  unmount() - Unmount a File System
 *
 *  This routine is not defined in the POSIX 1003.1b standard but
 *  in some form is supported on most UNIX and POSIX systems.  This
 *  routine is necessary to mount instantiations of a file system
 *  into the file system name space.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <errno.h>

#include <rtems/libio_.h>

int unmount( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  rtems_filesystem_mount_table_entry_t *mt_entry = currentloc->mt_entry;

  if ( rtems_filesystem_location_is_root( currentloc ) ) {
    rv = (*mt_entry->mt_point_node->location.ops->unmount_h)( mt_entry );
    if ( rv == 0 ) {
      rtems_filesystem_mt_entry_declare_lock_context( lock_context );

      rtems_filesystem_mt_entry_lock( lock_context );
      mt_entry->mounted = false;
      rtems_filesystem_mt_entry_unlock( lock_context );
    }
  } else {
    errno = EACCES;
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
