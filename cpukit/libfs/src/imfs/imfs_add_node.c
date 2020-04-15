/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Add a Node
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/imfs.h>

int IMFS_add_node( const char *path, IMFS_jnode_t *node, void *arg )
{
  mode_t                                  mode;
  rtems_filesystem_eval_path_context_t    ctx;
  const rtems_filesystem_location_info_t *currentloc;
  int                                     eval_flags;
  int                                     rv;

  mode = node->st_mode;
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
      return -1;
  }

  eval_flags = RTEMS_FS_FOLLOW_LINK;
  currentloc = rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  if ( IMFS_is_imfs_instance( currentloc ) ) {
    eval_flags = RTEMS_FS_MAKE | RTEMS_FS_EXCLUSIVE;
    rtems_filesystem_eval_path_set_flags( &ctx, eval_flags );
    rtems_filesystem_eval_path_set_path( &ctx, node->name, node->namelen );
    rtems_filesystem_eval_path_continue( &ctx );

    if ( rtems_filesystem_eval_path_get_token( &ctx ) == node->name ) {
      IMFS_assert(
        rtems_filesystem_eval_path_get_tokenlen( &ctx ) == node->namelen
      );
      node = IMFS_initialize_node(
        node,
        node->control,
        node->name,
        node->namelen,
        mode,
        arg
      );
      if ( node != NULL ) {
        IMFS_jnode_t *parent;

        currentloc = rtems_filesystem_eval_path_get_currentloc( &ctx );
        parent = currentloc->node_access;
        IMFS_assert( parent != NULL );
        IMFS_add_to_directory( parent, node );
        IMFS_mtime_ctime_update( parent );
        rv = 0;
      } else {
        rv = -1;
      }
    } else {
      if ( rtems_filesystem_eval_path_get_token( &ctx ) != NULL ) {
        rtems_filesystem_eval_path_error( &ctx, EINVAL );
      }

      rv = -1;
    }
  } else {
    rtems_filesystem_eval_path_error( &ctx, ENOTSUP );
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );
  return rv;
}
