/*
 *  This file contains the support infrastructure used to manage the
 *  table of integer style file descriptors used by the socket calls.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>             /* libio_.h pulls in rtems */
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

  if ((unsigned32)fd >= rtems_libio_number_iops) {
    errno = EBADF;
    return NULL;
  }
  iop = &rtems_libio_iops[fd];
  if (iop->data1 == NULL)
    errno = EBADF;
  return iop->data1;
}

/*
 * Create an RTEMS file descriptor for a socket
 */

int rtems_bsdnet_makeFdForSocket(
  void *so,
  const rtems_filesystem_file_handlers_r *h
)
{
  rtems_libio_t *iop;
  int fd;

  iop = rtems_libio_allocate();
  if (iop == 0) {
      errno = ENFILE;
      return -1;
  }
  fd = iop - rtems_libio_iops;
  iop->flags |= LIBIO_FLAGS_WRITE | LIBIO_FLAGS_READ;
  iop->data0 = fd;
  iop->data1 = so;
  iop->handlers = (rtems_filesystem_file_handlers_r *) h;
  return fd;
}
