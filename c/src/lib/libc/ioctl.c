/*
 *  ioctl() system call
 *
 *  This routine is not defined in the POSIX 1003.1b standard but is
 *  commonly supported on most UNIX and POSIX systems.
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

  /*
   *  If this file descriptor is mapped to an external set of handlers,
   *  then pass the request on to them.
   */ 

  if ( iop->flags & LIBIO_FLAGS_HANDLER_MASK ) {
    rtems_libio_ioctl_t fp;

    fp = rtems_libio_handlers[
           (iop->flags >> LIBIO_FLAGS_HANDLER_SHIFT) - 1].ioctl;
    if ( fp == NULL )
      set_errno_and_return_minus_one( EBADF );

    return (*fp)( fd, command, buffer );
  }

  /*
   *  Now process the ioctl().
   */

  if ( !iop->handlers->ioctl )
    set_errno_and_return_minus_one( ENOTSUP );

  rc = (*iop->handlers->ioctl)( iop, command, buffer );

  return rc;
} 
