/*  
 *  chdir() - POSIX 1003.1b - 5.2.1 - Change Current Working Directory
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

#include <rtems.h>

#include <unistd.h>
#include <errno.h>

#include "libio_.h"

int chdir(
  const char *pathname
)
{
  rtems_filesystem_location_info_t  loc;
  int                               result;

  /*
   *  Get the node where we wish to go.
   */

  result = rtems_filesystem_evaluate_path( 
    pathname, RTEMS_LIBIO_PERMS_SEARCH, &loc, TRUE );
  if ( result != 0 )
     return -1;

  /*
   * Verify you can change directory into this node.
   */

  if ( !loc.ops->node_type ) {
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type)( &loc ) != RTEMS_FILESYSTEM_DIRECTORY ) {
    rtems_filesystem_freenode( &loc );
    set_errno_and_return_minus_one( ENOTDIR );
  }
  
  rtems_filesystem_freenode( &rtems_filesystem_current );
   
  rtems_filesystem_current = loc;
  
  return 0;
}
