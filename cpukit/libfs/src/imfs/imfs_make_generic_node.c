/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

IMFS_jnode_t *IMFS_node_initialize_generic(
  IMFS_jnode_t *node,
  const IMFS_types_union *info
)
{
  node->info.generic.context = info->generic.context;

  return node;
}

bool IMFS_is_imfs_instance(
  const rtems_filesystem_location_info_t *loc
)
{
  const char *type = loc->mt_entry->type;

  return strcmp(type, RTEMS_FILESYSTEM_TYPE_IMFS) == 0
    || strcmp(type, RTEMS_FILESYSTEM_TYPE_MINIIMFS) == 0;
}

int IMFS_make_generic_node(
  const char *path,
  mode_t mode,
  const IMFS_node_control *node_control,
  void *context
)
{
  int rv = 0;

  mode &= ~rtems_filesystem_umask;

  switch (mode & S_IFMT) {
    case S_IFBLK:
    case S_IFCHR:
    case S_IFIFO:
    case S_IFREG:
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }
  
  if ( rv == 0 ) {
    if ( node_control->imfs_type == IMFS_GENERIC ) {
      rtems_filesystem_eval_path_context_t ctx;
      int eval_flags = RTEMS_FS_FOLLOW_LINK
        | RTEMS_FS_MAKE
        | RTEMS_FS_EXCLUSIVE;
      const rtems_filesystem_location_info_t *currentloc =
        rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

      if ( IMFS_is_imfs_instance( currentloc ) ) {
        IMFS_types_union info;
        IMFS_jnode_t *new_node;

        info.generic.context = context;
        new_node = IMFS_create_node_with_control(
          currentloc,
          node_control,
          rtems_filesystem_eval_path_get_token( &ctx ),
          rtems_filesystem_eval_path_get_tokenlen( &ctx ),
          mode,
          &info
        );

        if ( new_node != NULL ) {
          IMFS_jnode_t *parent = currentloc->node_access;

          IMFS_update_ctime( parent );
          IMFS_update_mtime( parent );
        } else {
          rv = -1;
        }
      } else {
        rtems_filesystem_eval_path_error( &ctx, ENOTSUP );
        rv = -1;
      }

      rtems_filesystem_eval_path_cleanup( &ctx );
    } else {
      errno = EINVAL;
      rv = -1;
    }
  }

  return rv;
}
