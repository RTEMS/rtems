/*
 *  fsync() - POSIX 1003.1b 6.6.1 - Synchronize the State of a File
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

#include <unistd.h>

#include "libio_.h"

int fsync(
  int     fd
)
{
  rtems_libio_t *iop;

  /*
   *  If this file descriptor is mapped to an external set of handlers,
   *  then pass the request on to them.
   */

  if ( rtems_file_descriptor_type( fd ) )
    set_errno_and_return_minus_one( EBADF );

  /*
   *  Now process the fsync().
   */

  iop = rtems_libio_iop( fd );
  rtems_libio_check_fd( fd );
  rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

  if ( !iop->handlers->fsync )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->fsync)( iop );
}
