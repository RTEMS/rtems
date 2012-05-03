/*
 *  IMFS_mknod
 *
 *  Routine to create a node in the IMFS file system.
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

#include "imfs.h"

static void get_type_and_info_by_mode_and_dev(
  mode_t mode,
  dev_t dev,
  IMFS_jnode_types_t *type,
  IMFS_types_union *info
)
{
  if ( S_ISDIR( mode ) ) {
    *type = IMFS_DIRECTORY;
  } else if ( S_ISREG( mode ) ) {
    *type = IMFS_MEMORY_FILE;
  } else if ( S_ISBLK( mode ) || S_ISCHR( mode ) ) {
    *type = IMFS_DEVICE;
    rtems_filesystem_split_dev_t(
      dev,
      info->device.major,
      info->device.minor
    );
  } else if (S_ISFIFO( mode )) {
    *type = IMFS_FIFO;
  } else {
    IMFS_assert( 0 );
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
  IMFS_jnode_types_t type;
  IMFS_types_union info;
  IMFS_jnode_t *new_node;

  get_type_and_info_by_mode_and_dev( mode, dev, &type, &info );

  new_node = IMFS_create_node( parentloc, type, name, namelen, mode, &info );
  if ( new_node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    IMFS_update_ctime( parent );
    IMFS_update_mtime( parent );
  } else {
    rv = -1;
  }

  return rv;
}
