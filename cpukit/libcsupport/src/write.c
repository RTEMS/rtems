/*
 *  write() - POSIX 1003.1b 6.4.2 - Write to a File
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


/*
 * write
 * 
 * This routine writes count bytes from from buffer pointed to by buffer
 * to the file associated with the open file descriptor, fildes.
 */

/* XXX newlib has the prototype for this wrong.  It will be a bit painful */
/* XXX to fix so we are choosing to delay fixing this. */

int write(                        /* XXX this should return a ssize_t */
  int         fd,
  const void *buffer,
  unsigned32  count               /* XXX this should be a size_t */
)
{
  rtems_status_code  rc;
  rtems_libio_t     *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_buffer( buffer );
  rtems_libio_check_count( count );
  rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

  /*
   *  If this file descriptor is mapped to an external set of handlers,
   *  then pass the request on to them.
   */

  if ( iop->flags & LIBIO_FLAGS_HANDLER_MASK ) {
    rtems_libio_write_t fp;

    fp = rtems_libio_handlers[
           (iop->flags >> LIBIO_FLAGS_HANDLER_SHIFT) - 1].write;
    if ( fp == NULL )
      set_errno_and_return_minus_one( EBADF );

    return (*fp)( fd, buffer, count );
  }

  /*
   *  Now process the write() request.
   */

  if ( !iop->handlers->write )
    set_errno_and_return_minus_one( ENOTSUP );

  rc = (*iop->handlers->write)( iop, buffer, count );

  if ( rc > 0 )
    iop->offset += rc;

  return rc;
}

/*
 *  _write_r
 *
 *  This is the Newlib dependent reentrant version of write().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

long _write_r(
  struct _reent *ptr,
  int            fd,
  const void    *buf,
  size_t         nbytes
)
{
  return write( fd, buf, nbytes );
}
#endif
