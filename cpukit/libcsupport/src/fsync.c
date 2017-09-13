/**
 *  @file
 *
 *  @brief Synchronize the State of a File
 *  @ingroup libcsupport
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

/**
 *  POSIX 1003.1b 6.6.1 - Synchronize the State of a File
 */
int fsync(
  int     fd
)
{
  rtems_libio_t *iop;
  int            rv;

  LIBIO_GET_IOP( fd, iop );

  /*
   *  Now process the fsync().
   */

  rv = (*iop->pathinfo.handlers->fsync_h)( iop );
  rtems_libio_iop_drop( iop );
  return rv;
}
