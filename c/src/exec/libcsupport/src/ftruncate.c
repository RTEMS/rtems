/*
 *  ftruncate() - Truncate a File to the Specified Length
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

#include <unistd.h>
#include <errno.h>

#include "libio_.h"

int ftruncate(
  int     fd,
  off_t   length
)
{
  rtems_libio_t                    *iop;
  rtems_filesystem_location_info_t  loc;
  
  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );

  /*
   *  If this is not a file system based entity, it is an error.
   */

  if ( iop->flags & LIBIO_FLAGS_HANDLER_MASK )
    set_errno_and_return_minus_one( EBADF );

  /*
   *  Now process the ftruncate() request.
   */
  
  /*
   *  Make sure we are not working on a directory
   */

  loc = iop->pathinfo;
  if ( !loc.ops->node_type )
    set_errno_and_return_minus_one( ENOTSUP );
    
  if ( (*loc.ops->node_type)( &loc ) == RTEMS_FILESYSTEM_DIRECTORY )
    set_errno_and_return_minus_one( EISDIR );

  rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

  if ( !iop->handlers->ftruncate )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->ftruncate)( iop, length );
}
  
