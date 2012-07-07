/*
 *  MSDOS directory handlers implementation
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

#include "msdos.h"

int
msdos_rmnod(const rtems_filesystem_location_info_t *parent_pathloc,
            const rtems_filesystem_location_info_t *pathloc)
{
    int                rc = RC_OK;
    msdos_fs_info_t   *fs_info = pathloc->mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = pathloc->node_access;

    if (fat_fd->fat_file_type == MSDOS_DIRECTORY)
    {
        bool is_empty = false;

        /*
         * You cannot remove a node that still has children
         */
        rc = msdos_dir_is_empty(pathloc->mt_entry, fat_fd, &is_empty);
        if (rc != RC_OK)
        {
            return rc;
        }

        if (!is_empty)
        {
            rtems_set_errno_and_return_minus_one(ENOTEMPTY);
        }

        /*
         * We deny attempts to delete open directory (if directory is current
         * directory we assume it is open one)
         */
        if (fat_fd->links_num > 1)
        {
            rtems_set_errno_and_return_minus_one(EBUSY);
        }

        /*
         * You cannot remove the file system root node.
         */
        if (rtems_filesystem_location_is_root(pathloc))
        {
            rtems_set_errno_and_return_minus_one(EBUSY);
        }

        /*
         * You cannot remove a mountpoint.
         * not used - mount() not implemenetd yet.
         */
    }

    /* mark file removed */
    rc = msdos_set_first_char4file_name(pathloc->mt_entry, &fat_fd->dir_pos,
                                        MSDOS_THIS_DIR_ENTRY_EMPTY);
    if (rc != RC_OK)
    {
        return rc;
    }

    fat_file_mark_removed(&fs_info->fat, fat_fd);

    return rc;
}
