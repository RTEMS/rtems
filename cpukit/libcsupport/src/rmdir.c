/*
 *  rmdir() - POSIX 1003.1b - 5.2.2 - Remove a Directory
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

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int rmdir(
  const char *pathname
)
{
  int                               parentpathlen;
  const char                       *name;
  rtems_filesystem_location_info_t  parentloc;
  rtems_filesystem_location_info_t  loc;
  int                               i;
  int                               result;

  /*
   *  Get the parent node of the node we wish to remove. Find the parent path.
   */

  parentpathlen = rtems_filesystem_dirname ( pathname );

  if ( parentpathlen == 0 )
    rtems_filesystem_get_start_loc( pathname, &i, &parentloc );
  else {
    result = rtems_filesystem_evaluate_path(pathname, parentpathlen,
                                            RTEMS_LIBIO_PERMS_WRITE,
                                            &parentloc,
                                            false );
    if ( result != 0 )
      return -1;
  }

  /*
   * Start from the parent to find the node that should be under it.
   */

  loc = parentloc;
  name = pathname + parentpathlen;
  name += rtems_filesystem_prefix_separators( name, strlen( name ) );

  result = rtems_filesystem_evaluate_relative_path( name , strlen( name ),
                                                    0, &loc, false );
  if ( result != 0 ) {
    rtems_filesystem_freenode( &parentloc );
    return -1;
  }

  /*
   * Verify you can remove this node as a directory.
   */

  if ( !loc.ops->node_type_h ){
    rtems_filesystem_freenode( &loc );
    rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type_h)( &loc ) != RTEMS_FILESYSTEM_DIRECTORY ){
    rtems_filesystem_freenode( &loc );
    rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( ENOTDIR );
  }

  /*
   * Use the filesystems rmnod to remove the node.
   */

  if ( !loc.handlers->rmnod_h ){
    rtems_filesystem_freenode( &loc );
    rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result =  (*loc.handlers->rmnod_h)( &parentloc, &loc );

  rtems_filesystem_freenode( &loc );
  rtems_filesystem_freenode( &parentloc );

  return result;
}
