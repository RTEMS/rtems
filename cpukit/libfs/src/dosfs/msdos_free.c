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
 *
 *  @(#) $Id$
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
 *
 * PARAMETERS:
 *     pathloc - node description
 *
 * RETURNS:
 *     RC_OK on success, or -1 code if error occured
 *
 */
int
msdos_free_node_info(rtems_filesystem_location_info_t *pathloc)
{
    int                rc = RC_OK; 
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = pathloc->mt_entry->fs_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    rc = fat_file_close(pathloc->mt_entry, pathloc->node_access);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;  
}
