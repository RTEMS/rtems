/*
 *  close() - POSIX 1003.1b 6.3.1 - Close a File
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

#include "libio_.h"

int close(
  int  fd
)
{
  rtems_libio_t      *iop;
  rtems_status_code   rc;
  int                 status;

  rtems_libio_check_fd(fd);
  iop = rtems_libio_iop(fd);
  rtems_libio_check_is_open(iop);
  if ( iop->flags & LIBIO_FLAGS_HANDLER_MASK ) {
    int (*fp)(int  fd);

    fp = rtems_libio_handlers[
            (iop->flags >> LIBIO_FLAGS_HANDLER_SHIFT) - 1].close;
    if ( fp == NULL )
      set_errno_and_return_minus_one( EBADF );
    status = (*fp)( fd );
    return status;
  }

  if ( !iop->handlers )
    set_errno_and_return_minus_one( EBADF );

  if ( !iop->handlers->close )
    set_errno_and_return_minus_one( ENOTSUP );

  rc = (*iop->handlers->close)( iop );

  rtems_libio_free( iop );

  if (rc != RTEMS_SUCCESSFUL)
    set_errno_and_return_minus_one( rc );

  return rc;
}

/*
 *  _close_r
 *
 *  This is the Newlib dependent reentrant version of close().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _close_r(
  struct _reent *ptr,
  int            fd
)
{
  return close( fd );
}
#endif
