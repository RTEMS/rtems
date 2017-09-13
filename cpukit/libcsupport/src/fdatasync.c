/**
 * @file
 *
 * @brief POSIX 1003.1b 6.6.2 - Synchronize the Data of a File
 * @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int fdatasync(
  int     fd
)
{
  rtems_libio_t *iop;
  int            rv;

  LIBIO_GET_IOP_WITH_ACCESS( fd, iop, LIBIO_FLAGS_WRITE, EBADF );

  /*
   *  Now process the fdatasync().
   */

  rv = (*iop->pathinfo.handlers->fdatasync_h)( iop );
  rtems_libio_iop_drop( iop );
  return rv;
}
