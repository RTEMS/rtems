/*
 *  utime() - POSIX 1003.1b 5.5.6 - Set File Access and Modification Times
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

/* FIXME: This include is a workaround for a broken <utime.h> in Newlib */
#include <sys/types.h>

#include <utime.h>

#include <rtems/libio_.h>

int utime( const char *path, const struct utimbuf *times )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  struct utimbuf now_times;

  if ( times == NULL ) {
    time_t now = time( NULL );

    now_times.actime = now;
    now_times.modtime = now;

    times = &now_times;
  }

  rv = (*currentloc->mt_entry->ops->utime_h)(
    currentloc,
    times->actime,
    times->modtime
  );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
