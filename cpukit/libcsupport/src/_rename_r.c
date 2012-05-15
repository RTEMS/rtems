/*
 *  _rename_r() - POSIX 1003.1b - 5.3.4 - Rename a file
 *
 *  COPYRIGHT (c) 1989-2007.
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

#if defined(RTEMS_NEWLIB) && !defined(HAVE__RENAME_R)

#include <stdio.h>

#include <rtems/libio_.h>

int _rename_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *old,
  const char    *new
)
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t old_ctx;
  int old_eval_flags = 0;
  rtems_filesystem_location_info_t old_parentloc;
  int old_parent_eval_flags = RTEMS_FS_PERMS_WRITE
    | RTEMS_FS_FOLLOW_HARD_LINK;
  const rtems_filesystem_location_info_t *old_currentloc =
    rtems_filesystem_eval_path_start_with_parent(
      &old_ctx,
      old,
      old_eval_flags,
      &old_parentloc,
      old_parent_eval_flags
    );
  rtems_filesystem_eval_path_context_t new_ctx;

  /* FIXME: This is not POSIX conform */
  int new_eval_flags = RTEMS_FS_FOLLOW_HARD_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE;

  const rtems_filesystem_location_info_t *new_currentloc =
    rtems_filesystem_eval_path_start( &new_ctx, new, new_eval_flags );

  rv = rtems_filesystem_location_exists_in_same_fs_instance_as(
    old_currentloc,
    new_currentloc
  );
  if ( rv == 0 ) {
    rv = (*new_currentloc->mt_entry->ops->rename_h)(
      &old_parentloc,
      old_currentloc,
      new_currentloc,
      rtems_filesystem_eval_path_get_token( &new_ctx ),
      rtems_filesystem_eval_path_get_tokenlen( &new_ctx )
    );
  }

  rtems_filesystem_eval_path_cleanup_with_parent( &old_ctx, &old_parentloc );
  rtems_filesystem_eval_path_cleanup( &new_ctx );

  return rv;
}
#endif
