/*
 *  readlink() - POSIX 1003.1b - X.X.X - XXX
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

int readlink(
  const char *pathname,
  char       *buf,
  int         bufsize
)
{
  rtems_filesystem_location_info_t  loc;
  int                               result;

  result = rtems_filesystem_evaluate_path( pathname, 0, &loc, FALSE );
  if ( result != 0 )
     return -1;
  
  if (!buf)
    set_errno_and_return_minus_one( EFAULT );

  if ( !loc.ops->node_type )
    set_errno_and_return_minus_one( ENOTSUP );

  if (  (*loc.ops->node_type)( &loc ) != RTEMS_FILESYSTEM_SYM_LINK )
    set_errno_and_return_minus_one( EINVAL );

  if ( !loc.ops->readlink )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*loc.ops->readlink)( &loc, buf, bufsize );
}
