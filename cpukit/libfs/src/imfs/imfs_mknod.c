/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief Create a IMFS Node
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
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
