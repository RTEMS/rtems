/*
 *  rtems_filesystem_evaluate_path()
 *
 *  Routine to seed the evaluate path routine.
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
#include "libio_.h"

int rtems_filesystem_evaluate_path(
  const char                        *pathname,
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc,
  int                                follow_link
)
{
  int                           i;
  int                           result;
  rtems_filesystem_node_types_t type;

  /*
   * Verify Input parameters.
   */

  if ( !pathname )
    set_errno_and_return_minus_one( EFAULT );

  if ( !pathloc )
    set_errno_and_return_minus_one( EIO );       /* should never happen */
  
  /*
   * Evaluate the path using the optable evalpath.
   */

  rtems_filesystem_get_start_loc( pathname, &i, pathloc );

  if ( !pathloc->ops->evalpath )
    set_errno_and_return_minus_one( ENOTSUP );

  result = (*pathloc->ops->evalpath)( &pathname[i], flags, pathloc );

  /*
   * Get the Node type and determine if you need to follow the link or
   * not.
   */

  if ( (result == 0) && follow_link ) {

    if ( !pathloc->ops->node_type )
      set_errno_and_return_minus_one( ENOTSUP );

    type = (*pathloc->ops->node_type)( pathloc );

    if ( ( type == RTEMS_FILESYSTEM_HARD_LINK ) ||
         ( type == RTEMS_FILESYSTEM_SYM_LINK ) ) {

        if ( !pathloc->ops->eval_link )
          set_errno_and_return_minus_one( ENOTSUP );

         result =  (*pathloc->ops->eval_link)( pathloc, flags );
 
    }
  }

  return result;
}

