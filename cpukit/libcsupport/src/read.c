/*
 *  read() - POSIX 1003.1b 6.4.1 - Read From a File
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

ssize_t read(
  int         fd,
  void       *buffer,
  size_t      count
)
{
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open( iop );
  rtems_libio_check_buffer( buffer );
  rtems_libio_check_count( count );
  rtems_libio_check_permissions_with_error( iop, LIBIO_FLAGS_READ, EBADF );

  /*
   *  Now process the read().
   */
  return (*iop->pathinfo.handlers->read_h)( iop, buffer, count );
}

/*
 *  _read_r
 *
 *  This is the Newlib dependent reentrant version of read().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE__READ_R)

#include <reent.h>

ssize_t _read_r(
  struct _reent *ptr __attribute__((unused)),
  int            fd,
  void          *buf,
  size_t         nbytes
)
{
  return read( fd, buf, nbytes );
}
#endif
