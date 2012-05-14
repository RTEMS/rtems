/*
 *  The following returns the type of node that the loc refers to.
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

/* msdos_node_type --
 *     Determine type of the node that the loc refers to.
 *
 * PARAMETERS:
 *     loc - node description
 *
 * RETURNS:
 *     node type
 *
 */
rtems_filesystem_node_types_t
msdos_node_type(const rtems_filesystem_location_info_t *loc)
{
    fat_file_fd_t *fat_fd;

    /*
     * we don't need to obtain the volume semaphore here because node_type_h
     * call always follows evalpath_h call(hence link increment occured) and
     * hence node_access memory can't be freed during processing node_type_h
     * call
     */
    fat_fd = loc->node_access;

    return fat_fd->fat_file_type;
}
