/*
 *  symlink() - POSIX 1003.1b - X.X.X - XXX
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

#include <rtems/libio_.h>

int symlink(
  const char *actualpath,
  const char *sympath
)
{
  rtems_filesystem_location_info_t    loc;
  int                                 i;
  const char                         *name_start;
  int                                 result;

  rtems_filesystem_get_start_loc( sympath, &i, &loc );
  result = (*loc.ops->evalformake_h)( &sympath[i], &loc, &name_start );
  if ( result != 0 )
    return -1;

  if ( !loc.ops->symlink_h ) {
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*loc.ops->symlink_h)( &loc, actualpath, name_start);

  rtems_filesystem_freenode( &loc );

  return result;
}

