/*
 *  stat() - POSIX 1003.1b 5.6.2 - Get File Status
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

#include <rtems.h>

#if !defined(RTEMS_UNIX)

#include <rtems/libio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "libio_.h"

int stat(
  const char  *path,
  struct stat *buf
)
{
  int                              status;
  rtems_filesystem_location_info_t loc;

  /*
   *  Check to see if we were passed a valid pointer.
   */

  if ( !buf )
    set_errno_and_return_minus_one( EFAULT );

  status = rtems_filesystem_evaluate_path( path, 0, &loc, TRUE );
  if ( status != 0 )
    return -1;
  
  if ( !loc.handlers->fstat )
    set_errno_and_return_minus_one( ENOTSUP );

  /*
   *  Zero out the stat structure so the various support
   *  versions of stat don't have to.
   */

  memset( buf, 0, sizeof(struct stat) );

  return (*loc.handlers->fstat)( &loc, buf );
}
#endif

/*
 *  _stat_r
 *
 *  This is the Newlib dependent reentrant version of stat().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _stat_r(
  struct _reent *ptr,
  const char    *path,
  struct stat   *buf
)
{
  return stat( path, buf );
}
#endif
