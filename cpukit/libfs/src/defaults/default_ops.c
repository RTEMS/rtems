/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief rtems_filesystem_operations_default definition.
 */

/*
 * Copyright (C) 2010 embedded brains GmbH & Co. KG
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

#include <rtems/libio.h>

const rtems_filesystem_operations_table rtems_filesystem_operations_default = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = rtems_filesystem_default_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = rtems_filesystem_default_mknod,
  .rmnod_h = rtems_filesystem_default_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = rtems_filesystem_default_clonenode,
  .freenod_h = rtems_filesystem_default_freenode,
  .mount_h = rtems_filesystem_default_mount,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utimens_h = rtems_filesystem_default_utimens,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};
