/*
 *  utime() - POSIX 1003.1b 5.5.6 - Set File Access and Modification Times
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <sys/types.h>
#include <utime.h>
#include <errno.h>

#include "libio_.h"

int utime(
  const char           *path,
  const struct utimbuf *times
)
{
  rtems_filesystem_location_info_t   temp_loc;
  int                                result;

  if ( rtems_filesystem_evaluate_path( path, 0x00, &temp_loc, TRUE ) )
    return -1;

  if ( !temp_loc.ops->utime )
    set_errno_and_return_minus_one( ENOTSUP );

  result = (*temp_loc.ops->utime)( &temp_loc, times->actime, times->modtime );

  rtems_filesystem_freenode( &temp_loc );

  return result;
}
