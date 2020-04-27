/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Make a Generic Node
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

#include <string.h>

IMFS_jnode_t *IMFS_node_initialize_generic(
  IMFS_jnode_t *node,
  void *arg
)
{
  IMFS_generic_t *generic = (IMFS_generic_t *) node;

  generic->context = arg;

  return node;
}

int IMFS_make_generic_node(
  const char              *path,
  mode_t                   mode,
  const IMFS_node_control *node_control,
  void                    *context
)
{
  return IMFS_make_node(
    path,
    mode,
    node_control,
    sizeof( IMFS_generic_t ),
    context
  );
}

int IMFS_make_node(
  const char              *path,
  mode_t                   mode,
  const IMFS_node_control *node_control,
  size_t                   node_size,
  void                    *context
)
{
  int rv = 0;

  mode &= ~rtems_filesystem_umask;

  switch (mode & S_IFMT) {
    case S_IFBLK:
    case S_IFCHR:
    case S_IFIFO:
    case S_IFREG:
    case S_IFSOCK:
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }

  if ( rv == 0 ) {
    rtems_filesystem_eval_path_context_t ctx;
    int eval_flags = RTEMS_FS_FOLLOW_LINK
      | RTEMS_FS_MAKE
      | RTEMS_FS_EXCLUSIVE;
    const rtems_filesystem_location_info_t *currentloc =
      rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

    if ( IMFS_is_imfs_instance( currentloc ) ) {
      IMFS_jnode_t *new_node = IMFS_create_node(
        currentloc,
        node_control,
        node_size,
        rtems_filesystem_eval_path_get_token( &ctx ),
        rtems_filesystem_eval_path_get_tokenlen( &ctx ),
        mode,
        context
      );

      if ( new_node != NULL ) {
        IMFS_jnode_t *parent = currentloc->node_access;

        IMFS_mtime_ctime_update( parent );
      } else {
        rv = -1;
      }
    } else {
      rtems_filesystem_eval_path_error( &ctx, ENOTSUP );
      rv = -1;
    }

    rtems_filesystem_eval_path_cleanup( &ctx );
  }

  return rv;
}
