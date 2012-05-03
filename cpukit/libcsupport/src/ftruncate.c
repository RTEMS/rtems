/*
 *  ftruncate() - Truncate a File to the Specified Length
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

#include <unistd.h>

#include <rtems/libio_.h>

int ftruncate( int fd, off_t length )
{
  int rv = 0;

  if ( length >= 0 ) {
    rtems_libio_t *iop;

    rtems_libio_check_fd( fd );
    iop = rtems_libio_iop( fd );
    rtems_libio_check_is_open( iop );
    rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

    rv = (*iop->pathinfo.handlers->ftruncate_h)( iop, length );
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}
