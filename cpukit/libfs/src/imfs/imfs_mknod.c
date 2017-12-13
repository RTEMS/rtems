/**
 * @file
 *
 * @brief Create a IMFS Node
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/imfs.h>

static const IMFS_mknod_control *get_control(
  const IMFS_mknod_controls *controls,
  mode_t mode
)
{
  if ( S_ISDIR( mode ) ) {
    return controls->directory;
  } else if ( S_ISBLK( mode ) || S_ISCHR( mode ) ) {
    return controls->device;
  } else if ( S_ISFIFO( mode ) ) {
    return controls->fifo;
  } else {
    IMFS_assert( S_ISREG( mode ) );
    return controls->file;
  }
}

int IMFS_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  int rv = 0;
  const IMFS_fs_info_t *fs_info = parentloc->mt_entry->fs_info;
  const IMFS_mknod_control *mknod_control =
    get_control( fs_info->mknod_controls, mode );
  IMFS_jnode_t *new_node;

  new_node = IMFS_create_node(
    parentloc,
    &mknod_control->node_control,
    mknod_control->node_size,
    name,
    namelen,
    mode,
    &dev
  );
  if ( new_node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    IMFS_mtime_ctime_update( parent );
  } else {
    rv = -1;
  }

  return rv;
}
