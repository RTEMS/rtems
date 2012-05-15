/*
 *  unlink() - POSIX 1003.1b - 5.5.1 - Remove an existing link
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

int unlink( const char *path )
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

  rv = (*ops->rmnod_h)( &parentloc, currentloc );

  rtems_filesystem_eval_path_cleanup_with_parent( &ctx, &parentloc );

  return rv;
}

/*
 *  _unlink_r
 *
 *  This is the Newlib dependent reentrant version of unlink().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _unlink_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *path
)
{
  return unlink( path );
}
#endif
