/*
 * Routine to rename a MSDOS filesystem node
 *
 * Copyright (C) 2010 Chris Johns <chrisj@rtems.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * @(#) $Id$
 *
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/libio_.h>
#include <time.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_rename --
 *     Rename the node by removing the exitsing directory entry and creating a
 *     new one.
 *
 * PARAMETERS:
 *     old_parent_loc     - node description for the "old parent" node
 *     old_loc            - node description for the "old" node
 *     new_parent_loc     - node description for the "parent" node
 *     name               - name of new node
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
msdos_rename(rtems_filesystem_location_info_t  *old_parent_loc,
             rtems_filesystem_location_info_t  *old_loc,
             rtems_filesystem_location_info_t  *new_parent_loc,
             const char                        *new_name)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info     = new_parent_loc->mt_entry->fs_info;
    fat_file_fd_t     *old_fat_fd  = old_loc->node_access;
    const char        *token;
    int                len;

    /*
     * check spelling and format new node name
     */
    if (MSDOS_NAME != msdos_get_token(new_name, strlen(new_name), &token, &len)) {
      rtems_set_errno_and_return_minus_one(ENAMETOOLONG);
    }
    /*
     * lock volume
     */
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_set_errno_and_return_minus_one(EIO);

    /*
     * create new directory entry as "hard link", copying relevant info from
     * existing file
     */
    rc = msdos_creat_node(new_parent_loc,
                          MSDOS_HARD_LINK,new_name,len,S_IFREG,
                          old_fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    /*
     * mark file removed
     */
    rc = msdos_set_first_char4file_name(old_loc->mt_entry,
                                        &old_fat_fd->dir_pos,
                                        MSDOS_THIS_DIR_ENTRY_EMPTY);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}
