/*
 *  lseek() - POSIX 1003.1b 6.5.3 - Reposition Read/Write File Offset
 *
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

#include <unistd.h>

#include <rtems/libio_.h>

off_t lseek( int fd, off_t offset, int whence )
{
  rtems_libio_t *iop;
  off_t          rv;

  LIBIO_GET_IOP( fd, iop );

  rv = (*iop->pathinfo.handlers->lseek_h)( iop, offset, whence );
  rtems_libio_iop_drop( iop );
  return rv;
}

/*
 *  _lseek_r
 *
 *  This is the Newlib dependent reentrant version of lseek().
 */

#if defined(RTEMS_NEWLIB) && !defined(HAVE__LSEEK_R)

#include <reent.h>

off_t _lseek_r(
  struct _reent *ptr RTEMS_UNUSED,
  int            fd,
  off_t          offset,
  int            whence
)
{
  return lseek( fd, offset, whence );
}
#endif
