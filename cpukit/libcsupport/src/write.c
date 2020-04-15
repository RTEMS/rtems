/**
 *  @file
 *
 *  @brief Write to a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/**
 *  POSIX 1003.1b 6.4.2 - Write to a File
 * 
 *  This routine writes count bytes from from buffer pointed to by buffer
 *  to the file associated with the open file descriptor, fildes.
 */
ssize_t write(
  int         fd,
  const void *buffer,
  size_t      count
)
{
  rtems_libio_t *iop;
  ssize_t        n;

  rtems_libio_check_buffer( buffer );
  rtems_libio_check_count( count );

  LIBIO_GET_IOP_WITH_ACCESS( fd, iop, LIBIO_FLAGS_WRITE, EBADF );

  /*
   *  Now process the write() request.
   */
  n = (*iop->pathinfo.handlers->write_h)( iop, buffer, count );
  rtems_libio_iop_drop( iop );
  return n;
}
