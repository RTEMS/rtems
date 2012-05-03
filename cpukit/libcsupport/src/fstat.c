/*
 *  fstat() - POSIX 1003.1b 5.6.2 - Get File Status
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

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
    rtems_set_errno_and_return_minus_one( EFAULT );

  /*
   *  Now process the stat() request.
   */
  iop = rtems_libio_iop( fd );
  rtems_libio_check_fd( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Zero out the stat structure so the various support
   *  versions of stat don't have to.
   */
  memset( sbuf, 0, sizeof(struct stat) );

  return (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, sbuf );
}

/*
 *  _fstat_r
 *
 *  This is the Newlib dependent reentrant version of fstat().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE_FSTAT_R)

#include <reent.h>

int _fstat_r(
  struct _reent *ptr __attribute__((unused)),
  int            fd,
  struct stat   *buf
)
{
  return fstat( fd, buf );
}
#endif
