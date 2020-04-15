/*
 *  writev() - POSIX 1003.1 - Write a Vector
 *
 *  OpenGroup URL:
 *
 *  http://www.opengroup.org/onlinepubs/009695399/functions/writev.html
 *
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

#include <sys/uio.h>

#include <rtems/libio_.h>

static ssize_t writev_adapter(
  rtems_libio_t      *iop,
  const struct iovec *iov,
  int                 iovcnt,
  ssize_t             total
)
{
  return ( *iop->pathinfo.handlers->writev_h )( iop, iov, iovcnt, total );
}

ssize_t writev(
  int                 fd,
  const struct iovec *iov,
  int                 iovcnt
)
{
  return rtems_libio_iovec_eval(
    fd,
    iov,
    iovcnt,
    LIBIO_FLAGS_WRITE,
    writev_adapter
  );
}
