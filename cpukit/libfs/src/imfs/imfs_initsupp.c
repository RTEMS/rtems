/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Node Support
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
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

#include <errno.h>

int IMFS_initialize_support(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  const IMFS_mount_data   *mount_data;
  IMFS_fs_info_t          *fs_info;
  const IMFS_node_control *node_control;
  IMFS_jnode_t            *root_node;

  mount_data = data;

  fs_info = mount_data->fs_info;
  fs_info->mknod_controls = mount_data->mknod_controls;
  node_control = &mount_data->mknod_controls->directory->node_control;
  root_node = &fs_info->Root_directory.Node;

  mt_entry->fs_info = fs_info;
  mt_entry->ops = mount_data->ops;
  mt_entry->pathconf_limits_and_options = &IMFS_LIMITS_AND_OPTIONS;
  mt_entry->mt_fs_root->location.node_access = root_node;
  mt_entry->mt_fs_root->location.handlers = node_control->handlers;

  root_node = IMFS_initialize_node(
    root_node,
    node_control,
    "",
    0,
    (S_IFDIR | 0755),
    NULL
  );
  IMFS_assert( root_node != NULL );

  return 0;
}

static IMFS_jnode_t *IMFS_node_initialize_enosys(
  IMFS_jnode_t *node,
  void *arg
)
{
  errno = ENOSYS;

  return NULL;
}

IMFS_jnode_t *IMFS_node_initialize_default(
  IMFS_jnode_t *node,
  void *arg
)
{
  return node;
}

const IMFS_mknod_control IMFS_mknod_control_enosys = {
  { .node_initialize = IMFS_node_initialize_enosys },
  .node_size = sizeof( IMFS_jnode_t )
};
