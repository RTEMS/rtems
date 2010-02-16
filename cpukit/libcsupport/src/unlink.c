/*
 *  unlink() - POSIX 1003.1b - 5.5.1 - Remove an existing link
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

#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int unlink(
  const char *path
)
{
  int                               parentpathlen;
  const char                       *name;
  rtems_filesystem_location_info_t  parentloc;
  rtems_filesystem_location_info_t  loc;
  int                               i;
  int                               result;
  bool                              free_parentloc = false;

  /*
   * Get the node to be unlinked. Find the parent path first.
   */

  parentpathlen = rtems_filesystem_dirname ( path );

  if ( parentpathlen == 0 )
    rtems_filesystem_get_start_loc( path, &i, &parentloc );
  else {
    result = rtems_filesystem_evaluate_path( path, parentpathlen,
                                             RTEMS_LIBIO_PERMS_WRITE,
                                             &parentloc,
                                             false );
    if ( result != 0 )
      return -1;

    free_parentloc = true;
  }

  /*
   * Start from the parent to find the node that should be under it.
   */

  loc = parentloc;
  name = path + parentpathlen;
  name += rtems_filesystem_prefix_separators( name, strlen( name ) );

  result = rtems_filesystem_evaluate_relative_path( name , strlen( name ),
                                                    0, &loc, false );
  if ( result != 0 ) {
    if ( free_parentloc )
      rtems_filesystem_freenode( &parentloc );
    return -1;
  }

  if ( !loc.ops->node_type_h ) {
    rtems_filesystem_freenode( &loc );
    if ( free_parentloc )
      rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type_h)( &loc ) == RTEMS_FILESYSTEM_DIRECTORY ) {
    rtems_filesystem_freenode( &loc );
    if ( free_parentloc )
      rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( EISDIR );
  }

  if ( !loc.ops->unlink_h ) {
    rtems_filesystem_freenode( &loc );
    if ( free_parentloc )
      rtems_filesystem_freenode( &parentloc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*loc.ops->unlink_h)( &parentloc, &loc );

  rtems_filesystem_freenode( &loc );
  if ( free_parentloc )
    rtems_filesystem_freenode( &parentloc );

  return result;
}

/*
 *  _unlink_r
 *
 *  This is the Newlib dependent reentrant version of unlink().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _unlink_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *path
)
{
  return unlink( path );
}
#endif
