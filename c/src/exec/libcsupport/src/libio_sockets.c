/*
 *  This file contains the support infrastructure used to manage the
 *  table of integer style file descriptors used by the socket calls.
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

#include "libio_.h"                   /* libio_.h pulls in rtems */
#include <rtems.h>

#include <errno.h>


/*
 * Convert an RTEMS file descriptor to a BSD socket pointer.
 */

struct socket *rtems_bsdnet_fdToSocket(
  int fd
)
{
  rtems_libio_t *iop;

  if ((unsigned32)fd >= rtems_libio_number_iops)
      return NULL;
  iop = &rtems_libio_iops[fd];
  if ((iop->flags & LIBIO_FLAGS_HANDLER_MASK) != LIBIO_FLAGS_HANDLER_SOCK)
      return NULL;
  return iop->data1;
}

/*
 * Create an RTEMS file descriptor for a socket
 */

int rtems_bsdnet_makeFdForSocket(
  void *so
)
{
  rtems_libio_t *iop;

  iop = rtems_libio_allocate();
  if (iop == 0) {
      errno = ENFILE;
      return -1;
  }
  iop->flags |= LIBIO_FLAGS_HANDLER_SOCK | LIBIO_FLAGS_WRITE | LIBIO_FLAGS_READ;
  iop->data1 = so;
  return iop - rtems_libio_iops;
}
