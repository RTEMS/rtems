/*
 *  symlink() - POSIX 1003.1b - X.X.X - XXX
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

int symlink( const char *path1, const char *path2 )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_HARD_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path2, eval_flags );

  rv = (*currentloc->mt_entry->ops->symlink_h)(
    currentloc,
    rtems_filesystem_eval_path_get_token( &ctx ),
    rtems_filesystem_eval_path_get_tokenlen( &ctx ),
    path1
  );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
