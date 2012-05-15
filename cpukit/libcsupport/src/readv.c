/*
 *  readv() - POSIX 1003.1 - Read a Vector
 *
 *  OpenGroup URL:
 *
 *  http://www.opengroup.org/onlinepubs/009695399/functions/readv.html
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

#include <sys/types.h>
#include <sys/uio.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

ssize_t readv(
  int                 fd,
  const struct iovec *iov,
  int                 iovcnt
)
{
  ssize_t        total;
  int            v;
  int            bytes;
  rtems_libio_t *iop;
  bool           all_zeros;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open( iop );
  rtems_libio_check_permissions_with_error( iop, LIBIO_FLAGS_READ, EBADF );

  /*
   *  Argument validation on IO vector
   */
  if ( !iov )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( iovcnt <= 0 )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( iovcnt > IOV_MAX )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  OpenGroup says that you are supposed to return EINVAL if the
   *  sum of the iov_len values in the iov array would overflow a
   *  ssize_t.
   *
   *  Also we would like to ensure that no IO is performed if there
   *  are obvious errors in the iovec.  So this extra loop ensures
   *  that we do not do anything if there is an argument error.
   */

  all_zeros = true;
  for ( total=0, v=0 ; v < iovcnt ; v++ ) {
    ssize_t old;

    /*
     *  iov[v].iov_len cannot be less than 0 because size_t is unsigned.
     *  So we only check for zero.
     */
    if ( iov[v].iov_base == 0 )
      rtems_set_errno_and_return_minus_one( EINVAL );

    /* check for wrap */
    old    = total;
    total += iov[v].iov_len;
    if ( total < old )
      rtems_set_errno_and_return_minus_one( EINVAL );

    if ( iov[v].iov_len )
      all_zeros = false;
  }

  /*
   *  A readv with all zeros logically has no effect.  Even though
   *  OpenGroup didn't address this case as they did with writev(),
   *  we will handle it the same way for symmetry.
   */
  if ( all_zeros == true ) {
    return 0;
  }

  /*
   *  Now process the readv().
   */
  for ( total=0, v=0 ; v < iovcnt ; v++ ) {
    bytes = (*iop->pathinfo.handlers->read_h)(
      iop,
      iov[v].iov_base,
      iov[v].iov_len
    );

    if ( bytes < 0 )
      return -1;

    if ( bytes > 0 ) {
      total       += bytes;
    }

    if (bytes != iov[ v ].iov_len)
      break;
  }

  return total;
}
