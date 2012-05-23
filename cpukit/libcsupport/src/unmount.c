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

static bool contains_root_or_current_directory(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  const rtems_filesystem_location_info_t *root =
    &rtems_filesystem_root->location;
  const rtems_filesystem_location_info_t *current =
    &rtems_filesystem_current->location;

  return mt_entry == root->mt_entry || mt_entry == current->mt_entry;
}

int unmount( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  rtems_filesystem_mount_table_entry_t *mt_entry = currentloc->mt_entry;

  if ( rtems_filesystem_location_is_root( currentloc ) ) {
    if ( !contains_root_or_current_directory( mt_entry ) ) {
      const rtems_filesystem_operations_table *mt_point_ops =
        mt_entry->mt_point_node->location.mt_entry->ops;

      rv = (*mt_point_ops->unmount_h)( mt_entry );
      if ( rv == 0 ) {
        rtems_id self_task_id = rtems_task_self();
        rtems_filesystem_mt_entry_declare_lock_context( lock_context );

        rtems_filesystem_mt_entry_lock( lock_context );
        mt_entry->unmount_task = self_task_id;
        mt_entry->mounted = false;
        rtems_filesystem_mt_entry_unlock( lock_context );
      }
    } else {
      errno = EBUSY;
      rv = -1;
    }
  } else {
    errno = EACCES;
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  if ( rv == 0 ) {
    rtems_event_set out;
    rtems_status_code sc = rtems_event_receive(
      RTEMS_FILESYSTEM_UNMOUNT_EVENT,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &out
    );

    if ( sc != RTEMS_SUCCESSFUL ) {
      rtems_fatal_error_occurred( 0xdeadbeef );
    }
  }

  return rv;
}
