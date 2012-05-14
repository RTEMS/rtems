/*
 *  File Operations Table for MSDOS filesystem
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

#include <rtems/libio.h>
#include "msdos.h"

const rtems_filesystem_file_handlers_r msdos_file_handlers = {
    rtems_filesystem_default_open,
    msdos_file_close,
    msdos_file_read,
    msdos_file_write,
    rtems_filesystem_default_ioctl,
    rtems_filesystem_default_lseek_file,
    msdos_file_stat,
    msdos_file_ftruncate,
    msdos_file_sync,
    msdos_sync,
    rtems_filesystem_default_fcntl
};
