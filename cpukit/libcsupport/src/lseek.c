/*
 *  lseek() - POSIX 1003.1b 6.5.3 - Reposition Read/Write File Offset
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

#include <stdio.h>

#include "libio_.h"

off_t lseek(
  int     fd,
  off_t   offset,
  int     whence
)
{
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  If this file descriptor is mapped to an external set of handlers,
   *  then pass the request on to them.
   */

  if ( iop->flags & LIBIO_FLAGS_HANDLER_MASK ) {
    rtems_libio_lseek_t fp;

    fp = rtems_libio_handlers[
           (iop->flags >> LIBIO_FLAGS_HANDLER_SHIFT) - 1].lseek;
    if ( fp == NULL )
      set_errno_and_return_minus_one( EBADF );

    return (*fp)( fd, offset, whence );
  }

  /*
   *  Now process the lseek().
   */

  switch ( whence ) {
    case SEEK_SET:
      iop->offset = offset;
      break;

    case SEEK_CUR:
      iop->offset += offset;
      break;

    case SEEK_END:
      iop->offset = iop->size - offset;
      break;

    default:
      errno = EINVAL;
      return -1;
  }

  if ( !iop->handlers->lseek )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->lseek)( iop, offset, whence );
}

/*
 *  _lseek_r
 *
 *  This is the Newlib dependent reentrant version of lseek().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

off_t _lseek_r( 
  struct _reent *ptr,
  int            fd,
  off_t          offset,
  int            whence
)
{
  return lseek( fd, offset, whence );
}
#endif

