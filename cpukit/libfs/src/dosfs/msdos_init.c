/*
 *  Init routine for MSDOS
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include "dosfs.h"
#include "msdos.h"

static int msdos_clone_node_info(rtems_filesystem_location_info_t *loc)
{
    fat_file_fd_t *fat_fd = loc->node_access;

    return fat_file_reopen(fat_fd);
}

const rtems_filesystem_operations_table  msdos_ops = {
  .lock_h         =  msdos_lock,
  .unlock_h       =  msdos_unlock,
  .eval_path_h    =  msdos_eval_path,
  .link_h         =  rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .node_type_h    =  msdos_node_type,
  .mknod_h        =  msdos_mknod,
  .rmnod_h        =  msdos_rmnod,
  .fchmod_h       =  rtems_filesystem_default_fchmod,
  .chown_h        =  rtems_filesystem_default_chown,
  .clonenod_h     =  msdos_clone_node_info,
  .freenod_h      =  msdos_free_node_info,
  .mount_h        =  rtems_filesystem_default_mount,
  .fsmount_me_h   =  rtems_dosfs_initialize,
  .unmount_h      =  rtems_filesystem_default_unmount,
  .fsunmount_me_h =  msdos_shut_down,
  .utime_h        =  rtems_filesystem_default_utime,
  .symlink_h      =  rtems_filesystem_default_symlink,
  .readlink_h     =  rtems_filesystem_default_readlink,
  .rename_h       =  msdos_rename,
  .statvfs_h      =  rtems_filesystem_default_statvfs
};

void msdos_lock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
  msdos_fs_info_t *fs_info = mt_entry->fs_info;
  rtems_status_code sc = rtems_semaphore_obtain(
    fs_info->vol_sema,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

void msdos_unlock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
  msdos_fs_info_t *fs_info = mt_entry->fs_info;
  rtems_status_code sc = rtems_semaphore_release(fs_info->vol_sema);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

/* msdos_initialize --
 *     MSDOS filesystem initialization. Called when mounting an
 *     MSDOS filesystem.
 *
 * PARAMETERS:
 *     temp_mt_entry - mount table entry
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 *
 */
int rtems_dosfs_initialize(rtems_filesystem_mount_table_entry_t *mt_entry,
                           const void                           *data)
{
    int rc;

    rc = msdos_initialize_support(mt_entry,
                                  &msdos_ops,
                                  &msdos_file_handlers,
                                  &msdos_dir_handlers);
    return rc;
}
