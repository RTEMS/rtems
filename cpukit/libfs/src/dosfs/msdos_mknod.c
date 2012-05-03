/*
 *  Routine for node creation in MSDOS filesystem.
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

int msdos_mknod(
    const rtems_filesystem_location_info_t *parentloc,
    const char *name,
    size_t namelen,
    mode_t mode,
    dev_t dev
)
{
    int                  rc = RC_OK;
    msdos_node_type_t    type = 0;

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
        rtems_set_errno_and_return_minus_one(EINVAL);

    /* Create an MSDOS node */
    rc = msdos_creat_node(parentloc, type, name, namelen, mode, NULL);

    return rc;
}
