/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief POSIX 1003.1b 6.3.1 - Close a File
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

int close(
  int  fd
)
{
  rtems_libio_t *iop;
  unsigned int   flags;
  int            rc;

  if ( (uint32_t) fd >= rtems_libio_number_iops ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  iop = rtems_libio_iop( fd );
  flags = rtems_libio_iop_flags( iop );

  while ( true ) {
    unsigned int desired;
    bool         success;

    if ( ( flags & LIBIO_FLAGS_OPEN ) == 0 ) {
      rtems_set_errno_and_return_minus_one( EBADF );
    }

    /* The expected flags */
    flags &= LIBIO_FLAGS_REFERENCE_INC - 1U;

    desired = flags & ~LIBIO_FLAGS_OPEN;
    success = _Atomic_Compare_exchange_uint(
      &iop->flags,
      &flags,
      desired,
      ATOMIC_ORDER_ACQ_REL,
      ATOMIC_ORDER_RELAXED
    );

    if ( success ) {
      break;
    }

    if ( ( flags & ~( LIBIO_FLAGS_REFERENCE_INC - 1U ) ) != 0 ) {
      rtems_set_errno_and_return_minus_one( EBUSY );
    }
  }

  rc = (*iop->pathinfo.handlers->close_h)( iop );

  rtems_libio_free( iop );

  return rc;
}

/*
 *  _close_r
 *
 *  This is the Newlib dependent reentrant version of close().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE__CLOSE_R)

#include <reent.h>

int _close_r(
  struct _reent *ptr RTEMS_UNUSED,
  int            fd
)
{
  return close( fd );
}
#endif
