/*
 *  chroot() -  Change Root Directory
 *  Author: fernando.ruiz@ctv.es
 *
 *  COPYRIGHT (c) 1989-2008.
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

int chroot( const char *path )
{
  int rv = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  rtems_filesystem_location_info_t loc;
  rtems_filesystem_global_location_t *new_current_loc;

  /*
   * We use the global environment for path evaluation.  This makes it possible
   * to escape from a chroot environment referencing an unmounted file system.
   */
  rtems_filesystem_eval_path_start_with_root_and_current(
    &ctx,
    path,
    eval_flags,
    &rtems_global_user_env.root_directory,
    &rtems_global_user_env.current_directory
  );

  rtems_filesystem_eval_path_extract_currentloc( &ctx, &loc );
  new_current_loc = rtems_filesystem_location_transform_to_global( &loc );
  if ( !rtems_filesystem_global_location_is_null( new_current_loc ) ) {
    rtems_filesystem_global_location_t *new_root_loc =
      rtems_filesystem_global_location_obtain( &new_current_loc );
    rtems_filesystem_node_types_t type =
      (*new_root_loc->location.mt_entry->ops->node_type_h)(
        &new_root_loc->location
      );

    if ( type == RTEMS_FILESYSTEM_DIRECTORY ) {
      sc = rtems_libio_set_private_env();
      if (sc == RTEMS_SUCCESSFUL) {
        rtems_filesystem_global_location_assign(
          &rtems_filesystem_root,
          new_root_loc
        );
        rtems_filesystem_global_location_assign(
          &rtems_filesystem_current,
          new_current_loc
        );
      } else {
        if (sc != RTEMS_UNSATISFIED) {
          errno = ENOMEM;
        }
        rv = -1;
      }
    } else {
      rtems_filesystem_location_error( &new_root_loc->location, ENOTDIR );
      rv = -1;
    }

    if ( rv != 0 ) {
      rtems_filesystem_global_location_release( new_root_loc );
    }
  } else {
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  if ( rv != 0 ) {
    rtems_filesystem_global_location_release( new_current_loc );
  }

  return rv;
}
