/*
 *  lseek() - POSIX 1003.1b 6.5.3 - Reposition Read/Write File Offset
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

#include <stdio.h>

#include "libio_.h"

off_t lseek(
  int     fd,
  off_t   offset,
  int     whence
)
{
  rtems_libio_t *iop;
  off_t          old_offset;
  off_t          status;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Check as many errors as possible before touching iop->offset.
   */

  if ( !iop->handlers->lseek )
    set_errno_and_return_minus_one( ENOTSUP );

  /*
   *  Now process the lseek().
   */

  old_offset = iop->offset;
  switch ( whence ) {
    case SEEK_SET:
      iop->offset = offset;
      break;

    case SEEK_CUR:
      iop->offset += offset;
      break;

    case SEEK_END:
      iop->offset = iop->size + offset;
      break;

    default:
      set_errno_and_return_minus_one( EINVAL );
  }

  /*
   *  At this time, handlers assume iop->offset has the desired
   *  new offset.
   */

  status = (*iop->handlers->lseek)( iop, offset, whence );
  if ( !status )
    return 0;

  /*
   *  So if the operation failed, we have to restore iop->offset.
   */

  iop->offset = old_offset;
  return status;
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

