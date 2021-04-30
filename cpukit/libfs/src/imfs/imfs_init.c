/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup LibFSIMFS
 *
 * @brief IMFS initialization.
 */

/*
 * COPYRIGHT (C) 1989, 2021 On-Line Applications Research Corporation (OAR).
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

#include <stdlib.h>

#include <rtems/seterr.h>

static const rtems_filesystem_operations_table IMFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = IMFS_eval_path,
  .link_h = IMFS_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = IMFS_mknod,
  .rmnod_h = IMFS_rmnod,
  .fchmod_h = IMFS_fchmod,
  .chown_h = IMFS_chown,
  .clonenod_h = IMFS_node_clone,
  .freenod_h = IMFS_node_free,
  .mount_h = IMFS_mount,
  .unmount_h = IMFS_unmount,
  .fsunmount_me_h = IMFS_fsunmount,
  .utimens_h = IMFS_utimens,
  .symlink_h = IMFS_symlink,
  .readlink_h = IMFS_readlink,
  .rename_h = IMFS_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const IMFS_mknod_controls IMFS_default_mknod_controls = {
  .directory = &IMFS_mknod_control_dir_default,
  .device = &IMFS_mknod_control_device,
  .file = &IMFS_mknod_control_memfile,
  .fifo = &IMFS_mknod_control_enosys
};

int IMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  IMFS_fs_info_t *fs_info = calloc( 1, sizeof( *fs_info ) );
  IMFS_mount_data mount_data = {
    .fs_info = fs_info,
    .ops = &IMFS_ops,
    .mknod_controls = &IMFS_default_mknod_controls
  };

  if ( fs_info == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  return IMFS_initialize_support( mt_entry, &mount_data );
}
