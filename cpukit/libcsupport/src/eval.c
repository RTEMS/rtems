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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

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
    rtems_set_errno_and_return_minus_one( EFAULT );

  if ( !pathloc )
    rtems_set_errno_and_return_minus_one( EIO );       /* should never happen */
  
  /*
   * Evaluate the path using the optable evalpath.
   */

  rtems_filesystem_get_start_loc( pathname, &i, pathloc );

  if ( !pathloc->ops->evalpath_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  result = (*pathloc->ops->evalpath_h)( &pathname[i], flags, pathloc );

  /*
   * Get the Node type and determine if you need to follow the link or
   * not.
   */

  if ( (result == 0) && follow_link ) {

    if ( !pathloc->ops->node_type_h ){
      rtems_filesystem_freenode( pathloc );
      rtems_set_errno_and_return_minus_one( ENOTSUP );
    }

    type = (*pathloc->ops->node_type_h)( pathloc );

    if ( ( type == RTEMS_FILESYSTEM_HARD_LINK ) ||
         ( type == RTEMS_FILESYSTEM_SYM_LINK ) ) {

        if ( !pathloc->ops->eval_link_h ){
          rtems_filesystem_freenode( pathloc );
          rtems_set_errno_and_return_minus_one( ENOTSUP );
        }

        /* what to do with the valid node pathloc points to
         * if eval_link_h fails?
         * Let the FS implementation deal with this case.  It
         * should probably free pathloc in either case:
         *  - if the link evaluation fails, it must free the
         *    original (valid) pathloc because we are going 
         *    to return -1 and hence the FS generics won't
         *    cleanup pathloc.
         *  - if the link evaluation is successful, the updated
         *    pathloc will be passed up (and eventually released).
         *    Hence, the (valid) originial node that we submit to
         *    eval_link_h() should be released by the handler.
         */

        result =  (*pathloc->ops->eval_link_h)( pathloc, flags );
 
    }
  }

  return result;
}


int rtems_filesystem_evaluate_parent(
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc
)
{
  rtems_filesystem_location_info_t  parent;
  int                               result;

  if ( !pathloc )
    rtems_set_errno_and_return_minus_one( EIO );       /* should never happen */
  
  if ( !pathloc->ops->evalpath_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  parent = *pathloc;
  result = (*pathloc->ops->evalpath_h)( "..", flags, &parent );
  if (result != 0){
    return -1;
  }
  rtems_filesystem_freenode( &parent );

  return result;
}

