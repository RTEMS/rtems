/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libfs_msdos MSDOS FileSystem
 *
 * @brief Rename a MSDOS FileSystem Node
 */

/*
 * Copyright (C) 2010 Chris Johns <chrisj@rtems.org>
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
    (void) old_parent_loc;

    int                rc = RC_OK;
    fat_file_fd_t     *old_fat_fd  = old_loc->node_access;
    fat_dir_pos_t      old_pos = old_fat_fd->dir_pos;

    /*
     * create new directory entry as "hard link", copying relevant info from
     * existing file
     */
    rc = msdos_creat_node(new_parent_loc,
                          FAT_HARD_LINK,new_name,new_namelen,S_IFREG,
                          old_fat_fd);
    if (rc != RC_OK)
    {
        return rc;
    }

    /*
     * mark file removed
     */
    rc = msdos_set_first_char4file_name(old_loc->mt_entry,
                                        &old_pos,
                                        MSDOS_THIS_DIR_ENTRY_EMPTY);

    return rc;
}
