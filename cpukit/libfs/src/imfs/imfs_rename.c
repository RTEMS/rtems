/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Rename
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <string.h>
#include <stdlib.h>

typedef struct {
  IMFS_node_control Base;
  const IMFS_node_control *replaced;
  char name[ RTEMS_ZERO_LENGTH_ARRAY ];
} IMFS_renamed_control;

static void IMFS_restore_replaced_control( IMFS_jnode_t *node )
{
  const IMFS_node_control *base;
  IMFS_renamed_control    *control;

  base = RTEMS_DECONST( IMFS_node_control *, node->control );
  control = (IMFS_renamed_control *) base;
  node->control = control->replaced;
  free( control );
}

static void IMFS_renamed_destroy( IMFS_jnode_t *node )
{
  IMFS_restore_replaced_control( node );
  ( *node->control->node_destroy )( node );
}

int IMFS_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
)
{
  IMFS_jnode_t         *node;
  IMFS_jnode_t         *new_parent;
  IMFS_renamed_control *control;

  /*
   * FIXME: Due to insufficient checks we can create inaccessible nodes with
   * this operation.
   */

  node = oldloc->node_access;
  new_parent = newparentloc->node_access;

  if ( node->Parent == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( namelen >= IMFS_NAME_MAX ) {
    rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
  }

  control = malloc( sizeof( *control ) + namelen );
  if ( control == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  memcpy( control->name, name, namelen );

  if ( node->control->node_destroy == IMFS_renamed_destroy ) {
    IMFS_restore_replaced_control( node );
  }

  control->Base = *node->control;
  control->Base.node_destroy = IMFS_renamed_destroy;
  control->replaced = node->control;
  node->control = &control->Base;
  node->name = control->name;
  node->namelen = namelen;

  IMFS_remove_from_directory( node );
  IMFS_add_to_directory( new_parent, node );
  IMFS_update_ctime( node );

  return 0;
}
