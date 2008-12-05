/*
 *  telldir() - XXX
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_TELLDIR

#include <sys/param.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

long telldir(
  DIR *dirp
)
{
  rtems_libio_t  *iop;

  if ( !dirp )
    rtems_set_errno_and_return_minus_one( EBADF );

  /*
   *  Get the file control block structure associated with the
   *  file descriptor
   */

  iop = rtems_libio_iop( dirp->dd_fd );

  if (iop == NULL)
     assert(0);

  return (long)( iop->offset );
}

#endif
