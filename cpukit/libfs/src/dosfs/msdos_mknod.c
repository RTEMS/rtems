/*
 *  Routine for node creation in MSDOS filesystem.
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  @(#) $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <rtems.h>

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_mknod --
 *     The following function checks spelling and formats name for a new node,
 *     determines type of the node to be created and creates it.
 *
 * PARAMETERS:
 *     token   - non-formatted name of a new node
 *     mode    - node type
 *     dev     - dev
 *     pathloc - parent directory description
 *
 * RETURNS:
 *     RC_OK on succes, or -1 if error occured and set errno
 *
 */
int 
msdos_mknod(
    const char                        *token,
    mode_t                             mode, 
    dev_t                              dev,
    rtems_filesystem_location_info_t  *pathloc 
    )
{
    int                  rc = RC_OK;  
    rtems_status_code    sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t     *fs_info = pathloc->mt_entry->fs_info; 
    msdos_token_types_t  type = 0;
    char                 new_name[ MSDOS_NAME_MAX + 1 ];
    int                  len;

    /* check spelling and format new node name */ 
    msdos_get_token(token, new_name, &len);

    /*
     *  Figure out what type of msdos node this is.
     */
    if (S_ISDIR(mode))
    {
       type = MSDOS_DIRECTORY;
    }   
    else if (S_ISREG(mode))
    {
        type = MSDOS_REGULAR_FILE;
    }  
    else 
        set_errno_and_return_minus_one(EINVAL);

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    /* Create an MSDOS node */
    rc = msdos_creat_node(pathloc, type, new_name, mode, NULL);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}
