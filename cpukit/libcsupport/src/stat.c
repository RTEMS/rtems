/*
 *  stat() - POSIX 1003.1b 5.6.2 - Get File Status
 *
 *  Reused from lstat().
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

/*
 *  lstat() and stat() share the same implementation with a minor
 *  difference on how links are evaluated.
 */

#ifndef _STAT_NAME
#define _STAT_NAME         stat
#define _STAT_R_NAME       _stat_r
#define _STAT_FOLLOW_LINKS RTEMS_FS_FOLLOW_LINK
#endif

#include <sys/stat.h>
#include <string.h>

#include <rtems/libio_.h>

int _STAT_NAME( const char *path, struct stat *buf )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = _STAT_FOLLOW_LINKS;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  memset( buf, 0, sizeof( *buf ) );

  rv = (*currentloc->handlers->fstat_h)( currentloc, buf );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}

/*
 *  _stat_r, _lstat_r
 *
 *  This is the Newlib dependent reentrant version of stat() and lstat().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _STAT_R_NAME(
  struct _reent *ptr __attribute__((unused)),
  const char    *path,
  struct stat   *buf
)
{
  return _STAT_NAME( path, buf );
}
#endif
