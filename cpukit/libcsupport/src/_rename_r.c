/*
 *  _rename_r() - POSIX 1003.1b - 5.3.4 - Rename a file
 *
 *  COPYRIGHT (c) 1989-2007.
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

#if defined(RTEMS_NEWLIB) && !defined(HAVE__RENAME_R)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int _rename_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *old,
  const char    *new
)
{
  int                                 old_parent_pathlen;
  rtems_filesystem_location_info_t    old_loc;
  rtems_filesystem_location_info_t    old_parent_loc;
  rtems_filesystem_location_info_t    new_parent_loc;
  int                                 i;
  int                                 result;
  const char                         *name;
  bool                                free_old_parentloc = false;

  /*
   *  Get the parent node of the old path to be renamed. Find the parent path.
   */

  old_parent_pathlen = rtems_filesystem_dirname ( old );

  if ( old_parent_pathlen == 0 )
    rtems_filesystem_get_start_loc( old, &i, &old_parent_loc );
  else {
    result = rtems_filesystem_evaluate_path( old, old_parent_pathlen,
                                             RTEMS_LIBIO_PERMS_WRITE,
                                             &old_parent_loc,
                                             false );
    if ( result != 0 )
      return -1;

    free_old_parentloc = true;
  }

  /*
   * Start from the parent to find the node that should be under it.
   */

  old_loc = old_parent_loc;
  name = old + old_parent_pathlen;
  name += rtems_filesystem_prefix_separators( name, strlen( name ) );

  result = rtems_filesystem_evaluate_relative_path( name , strlen( name ),
                                                    0, &old_loc, false );
  if ( result != 0 ) {
    if ( free_old_parentloc )
      rtems_filesystem_freenode( &old_parent_loc );
    rtems_set_errno_and_return_minus_one( result );
  }
  
  /*
   * Get the parent of the new node we are renaming to.
   */

  rtems_filesystem_get_start_loc( new, &i, &new_parent_loc );

  if ( !new_parent_loc.ops->evalformake_h ) {
    if ( free_old_parentloc )
      rtems_filesystem_freenode( &old_parent_loc );
    rtems_filesystem_freenode( &old_loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*new_parent_loc.ops->evalformake_h)( &new[i], &new_parent_loc, &name );
  if ( result != 0 ) {
    rtems_filesystem_freenode( &new_parent_loc );
    if ( free_old_parentloc )
      rtems_filesystem_freenode( &old_parent_loc );
    rtems_filesystem_freenode( &old_loc );
    rtems_set_errno_and_return_minus_one( result );
  }

  /*
   *  Check to see if the caller is trying to rename across file system
   *  boundaries.
   */

  if ( old_parent_loc.mt_entry != new_parent_loc.mt_entry ) {
    rtems_filesystem_freenode( &new_parent_loc );
    if ( free_old_parentloc )
      rtems_filesystem_freenode( &old_parent_loc );
    rtems_filesystem_freenode( &old_loc );
    rtems_set_errno_and_return_minus_one( EXDEV );
  }

  if ( !new_parent_loc.ops->rename_h ) {
    rtems_filesystem_freenode( &new_parent_loc );
    if ( free_old_parentloc )
      rtems_filesystem_freenode( &old_parent_loc );
    rtems_filesystem_freenode( &old_loc );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*new_parent_loc.ops->rename_h)( &old_parent_loc, &old_loc, &new_parent_loc, name );

  rtems_filesystem_freenode( &new_parent_loc );
  if ( free_old_parentloc )
    rtems_filesystem_freenode( &old_parent_loc );
  rtems_filesystem_freenode( &old_loc );

  return result;
}

#if 0
  struct stat sb;
  int s;

  s = stat( old, &sb);
  if ( s < 0 )
    return s;
  s = link( old, new );
  if ( s < 0 )
    return s;
  return S_ISDIR(sb.st_mode) ? rmdir( old ) : unlink( old );
#endif
                                            
#endif
