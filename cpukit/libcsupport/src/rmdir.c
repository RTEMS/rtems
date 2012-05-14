/*
 *  rmdir() - POSIX 1003.1b - 5.2.2 - Remove a Directory
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

int rmdir( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_REJECT_TERMINAL_DOT;
  rtems_filesystem_location_info_t parentloc;
  int parent_eval_flags = RTEMS_FS_PERMS_WRITE
    | RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start_with_parent(
      &ctx,
      path,
      eval_flags,
      &parentloc,
      parent_eval_flags
    );
  const rtems_filesystem_operations_table *ops = currentloc->mt_entry->ops;
  rtems_filesystem_node_types_t type = (*ops->node_type_h)( currentloc );

  if ( type == RTEMS_FILESYSTEM_DIRECTORY ) {
    rv = (*ops->rmnod_h)( &parentloc, currentloc );
  } else {
    rtems_filesystem_eval_path_error( &ctx, ENOTDIR );
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup_with_parent( &ctx, &parentloc );

  return rv;
}
