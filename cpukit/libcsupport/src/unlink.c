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
  rtems_filesystem_location_info_t  loc;
  int                               result;

  /*
   * Get the node to be unlinked.
   */

  result = rtems_filesystem_evaluate_path( path, 0, &loc, FALSE );
  if ( result != 0 )
     return -1;
  
  result = rtems_filesystem_evaluate_parent(RTEMS_LIBIO_PERMS_WRITE, &loc );
  if (result != 0){
    rtems_filesystem_freenode( &loc );
    return -1;
  }

  if ( !loc.ops->node_type_h ) {
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  if (  (*loc.ops->node_type_h)( &loc ) == RTEMS_FILESYSTEM_DIRECTORY ) {
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( EISDIR );
  }

  if ( !loc.ops->unlink_h ) {
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*loc.ops->unlink_h)( &loc );

  rtems_filesystem_freenode( &loc );
  
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
  struct _reent *ptr,
  const char    *path
)
{
  return unlink( path );
}
#endif

