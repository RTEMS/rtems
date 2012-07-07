/*
 *  Free node handler implementation for the filesystem
 *  operations table.
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

#include <rtems.h>
#include <rtems/libio_.h>

#include <errno.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_free_node_info --
 *     Call fat-file close routine.
 */
void
msdos_free_node_info(const rtems_filesystem_location_info_t *pathloc)
{
    msdos_fs_info_t *fs_info = pathloc->mt_entry->fs_info;

    fat_file_close(&fs_info->fat, pathloc->node_access);
}
