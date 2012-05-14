/*
 *  fchdir() - compatible with SVr4, 4.4BSD and X/OPEN - Change Directory
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

int fchdir( int fd )
{
  int rv = 0;
  rtems_libio_t *iop;
  struct stat st;
  rtems_filesystem_location_info_t loc;

  st.st_mode = 0;
  st.st_uid = 0;
  st.st_gid = 0;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open( iop );

  rtems_filesystem_instance_lock( &iop->pathinfo );
  rv = (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, &st );
  if ( rv == 0 ) {
    bool access_ok = rtems_filesystem_check_access(
      RTEMS_FS_PERMS_EXEC,
      st.st_mode,
      st.st_uid,
      st.st_gid
    );

    if ( access_ok ) {
      rtems_filesystem_location_clone( &loc, &iop->pathinfo );
    } else {
      errno = EACCES;
      rv = -1;
    }
  }
  rtems_filesystem_instance_unlock( &iop->pathinfo );

  if ( rv == 0 ) {
    rv = rtems_filesystem_chdir( &loc );
  }

  return rv;
}
