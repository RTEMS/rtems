/*
 *  utime() - POSIX 1003.1b 5.5.6 - Set File Access and Modification Times
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <utime.h>
#include <errno.h>

#include <rtems/libio_.h>

int utime(
  const char           *path,
  const struct utimbuf *times
)
{
  rtems_filesystem_location_info_t   temp_loc;
  int                                result;

  if ( rtems_filesystem_evaluate_path( path, 0x00, &temp_loc, TRUE ) )
    return -1;

  if ( !temp_loc.ops->utime_h ){
    rtems_filesystem_freenode( &temp_loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*temp_loc.ops->utime_h)( &temp_loc, times->actime, times->modtime );

  rtems_filesystem_freenode( &temp_loc );

  return result;
}
