/*
 *  Init routine for MSDOS
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

#include <rtems/libio_.h>
#include "msdos.h"

rtems_filesystem_operations_table  msdos_ops = {
    msdos_eval_path,
    msdos_eval4make,
    NULL,                 /* msdos_link */
    msdos_file_rmnod,    
    msdos_node_type,
    msdos_mknod,
    NULL,                 /* msdos_chown */
    msdos_free_node_info,
    NULL,
    msdos_initialize,
    NULL,
    msdos_shut_down,      /* msdos_shut_down */
    NULL,                 /* msdos_utime */
    NULL,
    NULL,
    NULL
};

/* msdos_initialize --
 *     MSDOS filesystem initialization 
 *
 * PARAMETERS:
 *     temp_mt_entry - mount table entry
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 *
 */
int 
msdos_initialize(rtems_filesystem_mount_table_entry_t *temp_mt_entry)
{
    int rc = RC_OK;
  
    rc = msdos_initialize_support(temp_mt_entry,
                                  &msdos_ops,
                                  &msdos_file_handlers,
                                  &msdos_dir_handlers);
    return rc;
}
