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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int mknod(
  const char *pathname,
  mode_t      mode,
  dev_t       dev
)
{
  rtems_filesystem_location_info_t    temp_loc;
  int                                 i;
  const char                         *name_start;
  int                                 result;

  if ( !(mode & (S_IFREG|S_IFCHR|S_IFBLK|S_IFIFO) ) )
    rtems_set_errno_and_return_minus_one( EINVAL );
  
  if ( S_ISFIFO(mode) )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  rtems_filesystem_get_start_loc( pathname, &i, &temp_loc );

  if ( !temp_loc.ops->evalformake_h ) {
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*temp_loc.ops->evalformake_h)( 
    &pathname[i],
    &temp_loc, 
    &name_start
  );
  if ( result != 0 )
    return -1;

  if ( !temp_loc.ops->mknod_h ) {
    rtems_filesystem_freenode( &temp_loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result =  (*temp_loc.ops->mknod_h)( name_start, mode, dev, &temp_loc );

  rtems_filesystem_freenode( &temp_loc );

  return result;
}
