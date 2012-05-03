/*
 *  chdir() - POSIX 1003.1b - 5.2.1 - Change Current Working Directory
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

int rtems_filesystem_chdir( rtems_filesystem_location_info_t *loc )
{
  int rv = 0;
  rtems_filesystem_global_location_t *global_loc =
    rtems_filesystem_location_transform_to_global( loc );
  rtems_filesystem_node_types_t type =
    rtems_filesystem_node_type( &global_loc->location );

  if ( type == RTEMS_FILESYSTEM_DIRECTORY ) {
    rtems_filesystem_global_location_assign(
      &rtems_filesystem_current,
      global_loc
    );
  } else {
    rtems_filesystem_location_error( &global_loc->location, ENOTDIR );
    rtems_filesystem_global_location_release( global_loc );
    rv = -1;
  }

  return rv;
}

int chdir( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  rtems_filesystem_location_info_t pathloc;

  rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  rtems_filesystem_eval_path_extract_currentloc( &ctx, &pathloc );
  rv = rtems_filesystem_chdir( &pathloc );
  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
