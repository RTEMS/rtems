/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
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

bool rtems_filesystem_check_access(
  int eval_flags,
  mode_t node_mode,
  uid_t node_uid,
  gid_t node_gid
)
{
  mode_t perm_flags = eval_flags & RTEMS_FS_PERMS_RWX;
  uid_t task_uid = geteuid();

  if (task_uid == 0 || task_uid == node_uid) {
    perm_flags <<= RTEMS_FS_USR_SHIFT;
  } else {
    gid_t task_gid = getegid();

    if (task_gid == 0 || task_gid == node_gid) {
      perm_flags <<= RTEMS_FS_GRP_SHIFT;
    } else {
      perm_flags <<= RTEMS_FS_OTH_SHIFT;
    }
  }

  return (perm_flags & node_mode) == perm_flags;
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
