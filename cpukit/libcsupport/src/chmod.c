/*
 *  chmod() - POSIX 1003.1b 5.6.4 - Change File Modes
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

#include <rtems.h>
#include <rtems/libio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int chmod(
  const char *path,
  mode_t      mode
)
{
  int                              status;
  rtems_filesystem_location_info_t loc;
  int                              result;

  status = rtems_filesystem_evaluate_path( path, 0, &loc, TRUE );
  if ( status != 0 )
    return -1;
  
  if ( !loc.handlers ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  if ( !loc.handlers->fchmod_h ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*loc.handlers->fchmod_h)( &loc, mode );

  rtems_filesystem_freenode( &loc );
  
  return result;
}
