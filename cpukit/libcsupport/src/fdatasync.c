/*
 *  fdatasync() - POSIX 1003.1b 6.6.2 - Synchronize the Data of a File
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

#include <unistd.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int fdatasync(
  int     fd
)
{
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);
  rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

  /*
   *  Now process the fdatasync().
   */

  if ( !iop->handlers->fdatasync_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->fdatasync_h)( iop );
}
