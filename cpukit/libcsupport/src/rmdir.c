/*
 *  rmdir() - POSIX 1003.1b - 5.2.2 - Remove a Directory
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

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/libio_.h>

int rmdir(
  const char *pathname
)
{
  rtems_filesystem_location_info_t  loc;
  int                               result;

  /*
   *  Get the node where we wish to go.
   */

  result = rtems_filesystem_evaluate_path( pathname, 0, &loc, FALSE );
  if ( result != 0 )
     return -1;

  /*
   * Verify you can remove this node as a directory.
   */

  if ( !loc.ops->node_type_h ){
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type_h)( &loc ) != RTEMS_FILESYSTEM_DIRECTORY ){
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTDIR );
  }

  /*
   * Use the filesystems rmnod to remove the node.
   */

  if ( !loc.handlers->rmnod_h ){
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  result =  (*loc.handlers->rmnod_h)( &loc );  

  rtems_filesystem_freenode( &loc );
  
  return result;
}
