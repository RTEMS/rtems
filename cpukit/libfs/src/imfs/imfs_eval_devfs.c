/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ClassicIO
 *
 * @brief Implementation of IMFS_eval_path_devfs().
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

#include <string.h>

typedef struct {
  uint8_t len;
  char name[3];
} IMFS_devfs_dir;

static const IMFS_devfs_dir IMFS_devfs_dirs[] = {
  { .len = 1, .name = { '/' } },
  { .len = 1, .name = { '.' } },
  { .len = 2, .name = { '.', '.' } },
  { .len = 3, .name = { 'd', 'e', 'v' } }
};

static IMFS_jnode_t *IMFS_devfs_is_dir(
  rtems_filesystem_eval_path_context_t *ctx,
  IMFS_directory_t                     *dir
)
{
  const char *path;
  size_t      pathlen;
  size_t      i;

  path = rtems_filesystem_eval_path_get_path( ctx );
  pathlen = rtems_filesystem_eval_path_get_pathlen( ctx );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( IMFS_devfs_dirs ); ++i ) {
    bool match;

    match = IMFS_devfs_dirs[ i ].len == pathlen
      && memcmp( IMFS_devfs_dirs[ i ].name, path, pathlen ) == 0;

    if ( match ) {
      int eval_flags;

      eval_flags = rtems_filesystem_eval_path_get_flags( ctx );
      eval_flags &= ~RTEMS_FS_EXCLUSIVE;
      rtems_filesystem_eval_path_set_flags( ctx, eval_flags );
      rtems_filesystem_eval_path_clear_path( ctx );
      return &dir->Node;
    }
  }

  return NULL;
}

static IMFS_jnode_t *IMFS_devfs_search(
  rtems_filesystem_eval_path_context_t *ctx,
  IMFS_directory_t                     *dir
)
{
  const char          *path;
  size_t               pathlen;
  rtems_chain_control *entries;
  rtems_chain_node    *current;
  rtems_chain_node    *tail;

  path = rtems_filesystem_eval_path_get_path( ctx );
  pathlen = rtems_filesystem_eval_path_get_pathlen( ctx );
  entries = &dir->Entries;
  current = rtems_chain_first( entries );
  tail = rtems_chain_tail( entries );

  while ( current != tail ) {
    IMFS_jnode_t *entry;
    bool          match;

    entry = (IMFS_jnode_t *) current;
    match = entry->namelen == pathlen
      && memcmp( entry->name, path, pathlen ) == 0;

    if ( match ) {
      return entry;
    }

    current = rtems_chain_next( current );
  }

  return NULL;
}

void IMFS_eval_path_devfs( rtems_filesystem_eval_path_context_t *ctx )
{
  rtems_filesystem_location_info_t *currentloc;
  IMFS_directory_t                 *dir;
  IMFS_jnode_t                     *entry;

  currentloc = rtems_filesystem_eval_path_get_currentloc( ctx );
  dir = currentloc->node_access;

  entry = IMFS_devfs_is_dir( ctx, dir );

  if ( entry != NULL ) {
    return;
  }

  entry = IMFS_devfs_search( ctx, dir );

  if ( entry != NULL ) {
    int eval_flags;

    eval_flags = rtems_filesystem_eval_path_get_flags( ctx );

    if ( ( eval_flags & RTEMS_FS_EXCLUSIVE ) == 0 ) {
      --dir->Node.reference_count;
      ++entry->reference_count;
      currentloc->node_access = entry;
      currentloc->node_access_2 = IMFS_generic_get_context_by_node( entry );
      IMFS_Set_handlers( currentloc );
      rtems_filesystem_eval_path_clear_path( ctx );
    } else {
      rtems_filesystem_eval_path_error( ctx, EEXIST );
    }
  } else {
    rtems_filesystem_eval_path_set_token(
      ctx,
      rtems_filesystem_eval_path_get_path( ctx ),
      rtems_filesystem_eval_path_get_pathlen( ctx )
    );
    rtems_filesystem_eval_path_clear_path( ctx );
  }
}
