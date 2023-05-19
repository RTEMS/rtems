/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Stat
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <rtems/imfs.h>

int IMFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_fs_info_t *fs_info = loc->mt_entry->fs_info;
  IMFS_jnode_t *the_jnode = loc->node_access;

  buf->st_dev   = rtems_filesystem_make_dev_t_from_pointer( fs_info );
  buf->st_mode  = the_jnode->st_mode;
  buf->st_nlink = the_jnode->st_nlink;
  buf->st_ino   = IMFS_node_to_ino( the_jnode );
  buf->st_uid   = the_jnode->st_uid;
  buf->st_gid   = the_jnode->st_gid;

  buf->st_atime = the_jnode->stat_atime;
  buf->st_mtime = the_jnode->stat_mtime;
  buf->st_ctime = the_jnode->stat_ctime;

  return 0;
}
