/*
 *  File Operations Table for MSDOS filesystem
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

#include <rtems/libio.h>
#include "msdos.h"

rtems_filesystem_file_handlers_r msdos_file_handlers = {
    msdos_file_open,
    msdos_file_close,
    msdos_file_read,
    msdos_file_write,
    msdos_file_ioctl,
    msdos_file_lseek,
    msdos_file_stat,
    NULL,
    msdos_file_ftruncate,
    NULL,                
    msdos_file_sync, 
    msdos_file_datasync,
    NULL,                 /* msdos_file_fcntl */
    msdos_file_rmnod
};
