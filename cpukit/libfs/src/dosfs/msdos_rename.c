/*
 * Routine to rename a MSDOS filesystem node
 *
 * Copyright (C) 2010 Chris Johns <chrisj@rtems.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
 */
int
msdos_rename(
    const rtems_filesystem_location_info_t *old_parent_loc,
    const rtems_filesystem_location_info_t *old_loc,
    const rtems_filesystem_location_info_t *new_parent_loc,
    const char *new_name,
    size_t new_namelen
)
{
    int                rc = RC_OK;
    fat_file_fd_t     *old_fat_fd  = old_loc->node_access;

    /*
     * create new directory entry as "hard link", copying relevant info from
     * existing file
     */
    rc = msdos_creat_node(new_parent_loc,
                          MSDOS_HARD_LINK,new_name,new_namelen,S_IFREG,
                          old_fat_fd);
    if (rc != RC_OK)
    {
        return rc;
    }

    /*
     * mark file removed
     */
    rc = msdos_set_first_char4file_name(old_loc->mt_entry,
                                        &old_fat_fd->dir_pos,
                                        MSDOS_THIS_DIR_ENTRY_EMPTY);

    return rc;
}
