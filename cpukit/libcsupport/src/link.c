/*
 *  link() - POSIX 1003.1b - 5.3.4 - Create a new link
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

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>

#include "libio_.h"

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
  result = (*parent_loc.ops->evalformake)( &new[i], &parent_loc, &name_start );
  if ( result != 0 )
     set_errno_and_return_minus_one( result );

  /*
   *  Check to see if the caller is trying to link across file system
   *  boundaries.
   */

  if ( parent_loc.mt_entry != existing_loc.mt_entry )
    set_errno_and_return_minus_one( EXDEV );

  if ( !parent_loc.ops->link )
    set_errno_and_return_minus_one( ENOTSUP );

  return (*parent_loc.ops->link)( &existing_loc, &parent_loc, name_start );
}
