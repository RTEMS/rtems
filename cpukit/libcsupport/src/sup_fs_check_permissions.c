/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS File System Permissions Check Support
 *  @ingroup LibIOInternal
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

#include <sys/stat.h>

#include <rtems/libio_.h>

#define RTEMS_FS_USR_SHIFT 6
#define RTEMS_FS_GRP_SHIFT 3
#define RTEMS_FS_OTH_SHIFT 0

RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_READ << RTEMS_FS_USR_SHIFT) == S_IRUSR,
  S_IRUSR
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_READ << RTEMS_FS_GRP_SHIFT) == S_IRGRP,
  S_IRGRP
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_READ << RTEMS_FS_OTH_SHIFT) == S_IROTH,
  S_IROTH
);

RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_WRITE << RTEMS_FS_USR_SHIFT) == S_IWUSR,
  S_IWUSR
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_WRITE << RTEMS_FS_GRP_SHIFT) == S_IWGRP,
  S_IWGRP
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_WRITE << RTEMS_FS_OTH_SHIFT) == S_IWOTH,
  S_IWOTH
);

RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_EXEC << RTEMS_FS_USR_SHIFT) == S_IXUSR,
  S_IXUSR
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_EXEC << RTEMS_FS_GRP_SHIFT) == S_IXGRP,
  S_IXGRP
);
RTEMS_STATIC_ASSERT(
  (RTEMS_FS_PERMS_EXEC << RTEMS_FS_OTH_SHIFT) == S_IXOTH,
  S_IXOTH
);

static bool equals_supplementary_group(
  const rtems_user_env_t *uenv,
  gid_t object_gid
)
{
  size_t i;

  for (i = 0; i < uenv->ngroups; ++i) {
    if (uenv->groups[i] == object_gid) {
      return true;
    }
  }

  return false;
}

bool rtems_filesystem_check_access(
  int flags,
  mode_t object_mode,
  uid_t object_uid,
  gid_t object_gid
)
{
  const rtems_user_env_t *uenv = rtems_current_user_env_get();
  mode_t access_flags = flags & RTEMS_FS_PERMS_RWX;
  uid_t task_uid = uenv->euid;

  if (task_uid == 0 || task_uid == object_uid) {
    access_flags <<= RTEMS_FS_USR_SHIFT;
  } else {
    gid_t task_gid = uenv->egid;

    if (
      task_gid == 0
        || task_gid == object_gid
        || equals_supplementary_group(uenv, object_gid)
    ) {
      access_flags <<= RTEMS_FS_GRP_SHIFT;
    } else {
      access_flags <<= RTEMS_FS_OTH_SHIFT;
    }
  }

  return (access_flags & object_mode) == access_flags;
}

bool rtems_filesystem_eval_path_check_access(
  rtems_filesystem_eval_path_context_t *ctx,
  int eval_flags,
  mode_t node_mode,
  uid_t node_uid,
  gid_t node_gid
)
{
  bool access_ok = rtems_filesystem_check_access(
    eval_flags,
    node_mode,
    node_uid,
    node_gid
  );

  if (!access_ok) {
    rtems_filesystem_eval_path_error(ctx, EACCES);
  }

  return access_ok;
}
