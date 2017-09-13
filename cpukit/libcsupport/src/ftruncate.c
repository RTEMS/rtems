/**
 *  @file
 *
 *  @brief Truncate a File to the Specified Length
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

    LIBIO_GET_IOP_WITH_ACCESS( fd, iop, LIBIO_FLAGS_WRITE, EINVAL );

    rv = (*iop->pathinfo.handlers->ftruncate_h)( iop, length );
    rtems_libio_iop_drop( iop );
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}
