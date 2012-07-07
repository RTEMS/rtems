/*
 *  MSDOS shut down handler implementation
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_shut_down --
 *     Shut down MSDOS filesystem - free all allocated resources (don't
 *     return if deallocation of some resource failed - free as much as
 *     possible).
 *
 * PARAMETERS:
 *     temp_mt_entry - mount table entry
 *
 */
void
msdos_shut_down(rtems_filesystem_mount_table_entry_t *temp_mt_entry)
{
    msdos_fs_info_t *fs_info = temp_mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = temp_mt_entry->mt_fs_root->location.node_access;

    /* close fat-file which correspondes to root directory */
    fat_file_close(&fs_info->fat, fat_fd);

    fat_shutdown_drive(&fs_info->fat);

    rtems_semaphore_delete(fs_info->vol_sema);
    free(fs_info->cl_buf);
    free(temp_mt_entry->fs_info);
}
