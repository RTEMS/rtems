/*
 *  fstat() - POSIX 1003.1b 5.6.2 - Get File Status
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

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "libio_.h"

int fstat(
  int          fd,
  struct stat *sbuf
)
{
  rtems_libio_t *iop;

  /*
   *  Check to see if we were passed a valid pointer.
   */

  if ( !sbuf )
    set_errno_and_return_minus_one( EFAULT );

  /*
   *  Zero out the stat structure so the various support
   *  versions of stat don't have to.
   */

  memset( sbuf, 0, sizeof(struct stat) );

  /*
   *  If this file descriptor is mapped to an external set of handlers,
   *  then pass the request on to them.
   */

  if (rtems_file_descriptor_type(fd)) {
    switch (rtems_file_descriptor_type (fd)) {
      case RTEMS_FILE_DESCRIPTOR_TYPE_FILE:
        break;

      case RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET:
#if !defined(__GO32__)
        sbuf->st_mode = S_IFSOCK;
        break;
#endif

      default:
        set_errno_and_return_minus_one( EBADF );
    }
  }

  /*
   *  Now process the stat() request.
   */

  iop = rtems_libio_iop( fd );
  rtems_libio_check_fd( fd );
  rtems_libio_check_is_open(iop);

  if ( !iop->handlers->fstat )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->fstat)( &iop->pathinfo, sbuf );
}

/*
 *  _fstat_r
 *
 *  This is the Newlib dependent reentrant version of fstat().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _fstat_r(
  struct _reent *ptr,
  int            fd,
  struct stat   *buf
)
{
  return fstat( fd, buf );
}
#endif
