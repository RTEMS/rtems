/*
 *  mknod()
 *
 *  This routine is not defined in the POSIX 1003.1b standard but is
 *  commonly supported on most UNIX and POSIX systems.  It is the
 *  foundation for creating file system objects.
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

#include <sys/stat.h>

#include <rtems/libio_.h>

int rtems_filesystem_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  int rv = 0;

  mode &= ~rtems_filesystem_umask;

  switch (mode & S_IFMT) {
    case S_IFBLK:
    case S_IFCHR:
    case S_IFDIR:
    case S_IFIFO:
    case S_IFREG:
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }
  
  if ( rv == 0 ) {
    const rtems_filesystem_operations_table *ops = parentloc->mt_entry->ops;

    rv = (*ops->mknod_h)( parentloc, name, namelen, mode, dev );
  }

  return rv;
}

int mknod( const char *path, mode_t mode, dev_t dev )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE
    | (S_ISDIR(mode) ? RTEMS_FS_ACCEPT_RESIDUAL_DELIMITERS : 0);
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  rv = rtems_filesystem_mknod(
    currentloc,
    rtems_filesystem_eval_path_get_token( &ctx ),
    rtems_filesystem_eval_path_get_tokenlen( &ctx ),
    mode,
    dev
  );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
