/*
 *  ftruncate() - Truncate a File to the Specified Length
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

#include <unistd.h>
#include <errno.h>

#include <rtems/libio_.h>

int ftruncate(
  int     fd,
  off_t   length
)
{
  rtems_libio_t                    *iop;
  rtems_filesystem_location_info_t  loc;
  
  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  /*
   *  Now process the ftruncate() request.
   */
  
  /*
   *  Make sure we are not working on a directory
   */

  loc = iop->pathinfo;
  if ( !loc.ops->node_type_h )
    set_errno_and_return_minus_one( ENOTSUP );
    
  if ( (*loc.ops->node_type_h)( &loc ) == RTEMS_FILESYSTEM_DIRECTORY )
    set_errno_and_return_minus_one( EISDIR );

  rtems_libio_check_permissions( iop, LIBIO_FLAGS_WRITE );

  if ( !iop->handlers->ftruncate_h )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->ftruncate_h)( iop, length );
}
  
