/*
 *  getdents() - Get Directory Entries
 *
 *  SVR4 and SVID extension required by Newlib readdir() family.
 *
 *  This routine will dd_len / (sizeof dirent) directory entries relative to
 *  the current directory position index. These entries will be placed in
 *  character array pointed to by -dd_buf-
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

#include <errno.h>

#include <rtems/libio_.h>

int getdents(
  int   dd_fd,
  char *dd_buf,
  int   dd_len
)
{
  rtems_libio_t                    *iop;
  rtems_filesystem_location_info_t  loc;

  /*
   *  Get the file control block structure associated with the file descriptor
   */

  iop = rtems_libio_iop( dd_fd );

  /*
   *  Make sure we are working on a directory
   */
  loc = iop->pathinfo;
  if ( !loc.ops->node_type_h )
    set_errno_and_return_minus_one( ENOTSUP );
    
  if ( (*loc.ops->node_type_h)( &loc ) != RTEMS_FILESYSTEM_DIRECTORY )
    set_errno_and_return_minus_one( ENOTDIR );

  /*
   *  Return the number of bytes that were actually transfered as a result
   *  of the read attempt.
   */

  if ( !iop->handlers->read_h )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*iop->handlers->read_h)( iop, dd_buf, dd_len  );
}
