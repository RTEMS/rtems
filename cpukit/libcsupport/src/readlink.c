/**
 *  @file
 *
 *  @brief Read Value of a Symbolic Link 
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

/**
 *  POSIX 1003.1b - X.X.X - XXX
 */
ssize_t readlink( const char *__restrict path, char *__restrict buf,
                  size_t bufsize )
{
  ssize_t rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_HARD_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  const rtems_filesystem_operations_table *ops = currentloc->mt_entry->ops;
  mode_t type = rtems_filesystem_location_type( currentloc );

  if ( S_ISLNK( type ) ) {
    rv = (*ops->readlink_h)( currentloc, buf, bufsize );
  } else {
    rtems_filesystem_eval_path_error( &ctx, EINVAL );
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
