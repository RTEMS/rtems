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

#include "imfs.h"

static IMFS_jnode_types_t get_type( mode_t mode )
{
  if ( S_ISDIR( mode ) ) {
    return IMFS_DIRECTORY;
  } else if ( S_ISBLK( mode ) || S_ISCHR( mode ) ) {
    return IMFS_DEVICE;
  } else if (S_ISFIFO( mode )) {
    return IMFS_FIFO;
  } else {
    IMFS_assert( S_ISREG( mode ) );
    return IMFS_MEMORY_FILE;
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
  IMFS_jnode_t *new_node;

  type = get_type( mode );

  new_node = IMFS_create_node( parentloc, type, name, namelen, mode, &dev );
  if ( new_node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    IMFS_mtime_ctime_update( parent );
  } else {
    rv = -1;
  }

  return rv;
}
