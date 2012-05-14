/*
 *  write() - POSIX 1003.1b 6.4.2 - Write to a File
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 * write
 *
 * This routine writes count bytes from from buffer pointed to by buffer
 * to the file associated with the open file descriptor, fildes.
 */

ssize_t write(
  int         fd,
  const void *buffer,
  size_t      count
)
{
  rtems_libio_t     *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open( iop );
  rtems_libio_check_buffer( buffer );
  rtems_libio_check_count( count );
  rtems_libio_check_permissions_with_error( iop, LIBIO_FLAGS_WRITE, EBADF );

  /*
   *  Now process the write() request.
   */
  return (*iop->pathinfo.handlers->write_h)( iop, buffer, count );
}
