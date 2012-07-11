/*
 *  MSDOS Initialization support routine implementation
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

/* msdos_initialize_support --
 *     MSDOS filesystem initialization
 *
 * PARAMETERS:
 *     temp_mt_entry      - mount table entry
 *     op_table           - filesystem operations table
 *     file_handlers      - file operations table
 *     directory_handlers - directory operations table
 *
 * RETURNS:
 *     RC_OK and filled temp_mt_entry on success, or -1 if error occured
 *     (errno set apropriately)
 *
 */
int
msdos_initialize_support(
    rtems_filesystem_mount_table_entry_t    *temp_mt_entry,
    const rtems_filesystem_operations_table *op_table,
    const rtems_filesystem_file_handlers_r  *file_handlers,
    const rtems_filesystem_file_handlers_r  *directory_handlers
    )
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = NULL;
    fat_file_fd_t     *fat_fd = NULL;
    fat_dir_pos_t      root_pos;
    uint32_t           cl_buf_size;

    fs_info = (msdos_fs_info_t *)calloc(1, sizeof(msdos_fs_info_t));
    if (!fs_info)
        rtems_set_errno_and_return_minus_one(ENOMEM);

    temp_mt_entry->fs_info = fs_info;

    rc = fat_init_volume_info(&fs_info->fat, temp_mt_entry->dev);
    if (rc != RC_OK)
    {
        free(fs_info);
        return rc;
    }

    fs_info->file_handlers      = file_handlers;
    fs_info->directory_handlers = directory_handlers;

    /*
     * open fat-file which correspondes to  root directory
     * (so inode number 0x00000010 is always used for root directory)
     */
    fat_dir_pos_init(&root_pos);
    root_pos.sname.cln = FAT_ROOTDIR_CLUSTER_NUM;
    rc = fat_file_open(&fs_info->fat, &root_pos, &fat_fd);
    if (rc != RC_OK)
    {
        fat_shutdown_drive(&fs_info->fat);
        free(fs_info);
        return rc;
    }

    /* again: unfortunately "fat-file" is just almost fat file :( */
    fat_fd->fat_file_type = FAT_DIRECTORY;
    fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;
    fat_fd->cln = fs_info->fat.vol.rdir_cl;

    fat_fd->map.file_cln = 0;
    fat_fd->map.disk_cln = fat_fd->cln;

    /* if we have FAT12/16 */
    if ( fat_fd->cln == 0 )
    {
        fat_fd->fat_file_size = fs_info->fat.vol.rdir_size;
        cl_buf_size = (fs_info->fat.vol.bpc > fs_info->fat.vol.rdir_size) ?
                      fs_info->fat.vol.bpc                                :
                      fs_info->fat.vol.rdir_size;
    }
    else
    {
        rc = fat_file_size(&fs_info->fat, fat_fd);
        if ( rc != RC_OK )
        {
            fat_file_close(&fs_info->fat, fat_fd);
            fat_shutdown_drive(&fs_info->fat);
            free(fs_info);
            return rc;
        }
        cl_buf_size = fs_info->fat.vol.bpc;
    }

    fs_info->cl_buf = (uint8_t *)calloc(cl_buf_size, sizeof(char));
    if (fs_info->cl_buf == NULL)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        fat_shutdown_drive(&fs_info->fat);
        free(fs_info);
        rtems_set_errno_and_return_minus_one(ENOMEM);
    }

    sc = rtems_semaphore_create(3,
                                1,
                                RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
                                0,
                                &fs_info->vol_sema);
    if (sc != RTEMS_SUCCESSFUL)
    {
        fat_file_close(&fs_info->fat, fat_fd);
        fat_shutdown_drive(&fs_info->fat);
        free(fs_info->cl_buf);
        free(fs_info);
        rtems_set_errno_and_return_minus_one( EIO );
    }

    temp_mt_entry->mt_fs_root->location.node_access = fat_fd;
    temp_mt_entry->mt_fs_root->location.handlers = directory_handlers;
    temp_mt_entry->ops = op_table;

    return rc;
}
