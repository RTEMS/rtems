/*
 *  link() - POSIX 1003.1b - 5.3.4 - Create a new link
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

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int link(
  const char *existing,
  const char *new
)
{
  rtems_filesystem_location_info_t    existing_loc;
  rtems_filesystem_location_info_t    parent_loc;
  int                                 i;
  int                                 result;
  const char                         *name_start;

  /*
   * Get the node we are linking to.
   */

  result = rtems_filesystem_evaluate_path( existing, 0, &existing_loc, TRUE );
  if ( result != 0 )
     return -1;

  /*
   * Get the parent of the node we are creating.
   */

  rtems_filesystem_get_start_loc( new, &i, &parent_loc );

  if ( !parent_loc.ops->evalformake_h ) {
    rtems_filesystem_freenode( &existing_loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*parent_loc.ops->evalformake_h)( &new[i], &parent_loc, &name_start );
  if ( result != 0 ) {
    rtems_filesystem_freenode( &existing_loc );
    rtems_set_errno_and_return_minus_one( result );
  }

  /*
   *  Check to see if the caller is trying to link across file system
   *  boundaries.
   */

  if ( parent_loc.mt_entry != existing_loc.mt_entry ) {
    rtems_filesystem_freenode( &existing_loc );
    rtems_filesystem_freenode( &parent_loc );
    rtems_set_errno_and_return_minus_one( EXDEV );
  }

  if ( !parent_loc.ops->link_h ) {
    rtems_filesystem_freenode( &existing_loc );
    rtems_filesystem_freenode( &parent_loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*parent_loc.ops->link_h)( &existing_loc, &parent_loc, name_start );
  
  rtems_filesystem_freenode( &existing_loc );
  rtems_filesystem_freenode( &parent_loc );

  return result;
}

/*
 *  _link_r
 *
 *  This is the Newlib dependent reentrant version of link().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _link_r(
  struct _reent *ptr,
  const char    *existing,
  const char    *new
)
{
  return link( existing, new );
}
#endif

