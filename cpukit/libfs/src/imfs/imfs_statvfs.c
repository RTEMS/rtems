/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief FIFO Support
 */

/*
 * Copyright (c) 2025 On-Line Application Research Corporation (OAR)
 * Copyright (c) 2025 Bhavya Shah <bhavyashah8443@gmail.com>
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

#include <rtems/imfsimpl.h>
#include <limits.h>
#include <rtems/libcsupport.h>

int IMFS_statvfs(
  const rtems_filesystem_location_info_t *loc,
  struct statvfs                         *buf
)
{
  IMFS_fs_info_t *fs_info = loc->mt_entry->fs_info;
  buf->f_bsize = IMFS_MEMFILE_BYTES_PER_BLOCK;
  buf->f_frsize = IMFS_MEMFILE_BYTES_PER_BLOCK;
  buf->f_blocks = UINT_MAX / IMFS_MEMFILE_BYTES_PER_BLOCK;
  buf->f_bfree = imfs_memfile_ops.get_free_space() /
                 IMFS_MEMFILE_BYTES_PER_BLOCK;
  buf->f_bavail = imfs_memfile_ops.get_free_space() /
                  IMFS_MEMFILE_BYTES_PER_BLOCK;
  buf->f_files = fs_info->jnode_count;
  buf->f_fsid = 1;
  buf->f_flag = loc->mt_entry->writeable;
  buf->f_namemax = IMFS_NAME_MAX;
  return 0;
}
