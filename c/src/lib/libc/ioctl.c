/*
 *  ioctl() system call
 *
 *  This routine is not defined in the POSIX 1003.1b standard but is
 *  commonly supported on most UNIX and POSIX systems.
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

#include "libio_.h"

int ioctl(
  int         fd,
  unsigned32  command,
  void *      buffer
) 
{ 
  rtems_status_code  rc;
  rtems_libio_t     *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Now process the ioctl().
   */

  if ( !iop->handlers )
    set_errno_and_return_minus_one( EBADF );

  if ( !iop->handlers->ioctl )
    set_errno_and_return_minus_one( ENOTSUP );

  rc = (*iop->handlers->ioctl)( iop, command, buffer );

  return rc;
} 
