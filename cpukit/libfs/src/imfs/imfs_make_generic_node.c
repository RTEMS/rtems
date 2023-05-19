/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Make a Generic Node
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfsimpl.h>

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
