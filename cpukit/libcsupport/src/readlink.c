/*
 *  readlink() - POSIX 1003.1b - X.X.X - XXX
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int readlink(
  const char *pathname,
  char       *buf,
  int         bufsize
)
{
  rtems_filesystem_location_info_t  loc;
  int                               result;

  if (!buf)
    rtems_set_errno_and_return_minus_one( EFAULT );

  result = rtems_filesystem_evaluate_path( pathname, 0, &loc, FALSE );
  if ( result != 0 )
     return -1;
  
  if ( !loc.ops->node_type_h ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type_h)( &loc ) != RTEMS_FILESYSTEM_SYM_LINK ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( !loc.ops->readlink_h ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result =  (*loc.ops->readlink_h)( &loc, buf, bufsize );

  rtems_filesystem_freenode( &loc );
  
  return result;
}
