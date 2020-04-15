/**
 * @file
 *
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

IMFS_jnode_t *IMFS_node_initialize_directory(
  IMFS_jnode_t *node,
  void *arg
)
{
  IMFS_directory_t *dir = (IMFS_directory_t *) node;

  rtems_chain_initialize_empty( &dir->Entries );

  return node;
}

static bool IMFS_is_mount_point( const IMFS_directory_t *dir )
{
  return dir->mt_fs != NULL;
}

IMFS_jnode_t *IMFS_node_remove_directory( IMFS_jnode_t *node )
{
  IMFS_directory_t *dir = (IMFS_directory_t *) node;

  if ( !rtems_chain_is_empty( &dir->Entries ) ) {
    errno = ENOTEMPTY;
    dir = NULL;
  } else if ( IMFS_is_mount_point( dir ) ) {
    errno = EBUSY;
    dir = NULL;
  }

  return &dir->Node;
}
