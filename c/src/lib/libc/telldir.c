/*
 *  telldir() - XXX
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

#include <sys/param.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include "libio_.h"


long telldir(
  DIR *dirp
)
{
  rtems_libio_t  *iop;

  /*
   *  Get the file control block structure associated with the file descriptor
   */

  iop = rtems_libio_iop( dirp->dd_fd );

  if (iop == NULL)
     assert(0);

  return (long)( iop->offset );
}
