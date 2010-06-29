/*
 *  Init routine for MSDOS
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  @(#) $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>
#include "dosfs.h"
#include "msdos.h"

const rtems_filesystem_operations_table  msdos_ops = {
  .evalpath_h     =  msdos_eval_path,
  .evalformake_h  =  msdos_eval4make,
  .link_h         =  rtems_filesystem_default_link,
  .unlink_h       =  msdos_file_rmnod,
  .node_type_h    =  msdos_node_type,
  .mknod_h        =  msdos_mknod,
  .chown_h        =  rtems_filesystem_default_chown,
  .freenod_h      =  msdos_free_node_info,
  .mount_h        =  rtems_filesystem_default_mount,
  .fsmount_me_h   =  rtems_dosfs_initialize,
  .unmount_h      =  rtems_filesystem_default_unmount,
  .fsunmount_me_h =  msdos_shut_down,
  .utime_h        =  rtems_filesystem_default_utime,
  .eval_link_h    =  rtems_filesystem_default_evaluate_link,
  .symlink_h      =  rtems_filesystem_default_symlink,
  .readlink_h     =  rtems_filesystem_default_readlink,
  .rename_h       =  msdos_rename,
  .statvfs_h      =  rtems_filesystem_default_statvfs
};

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
