/*
 *  fat_fat_operations.h
 *
 *  Constants/data structures/prototypes for operations on Files Allocation 
 *  Table
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
#ifndef __DOSFS_FAT_FAT_OPERATIONS_H__
#define __DOSFS_FAT_FAT_OPERATIONS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio_.h>

#include <rtems/bdbuf.h>
#include "fat.h"

int
fat_get_fat_cluster(rtems_filesystem_mount_table_entry_t *mt_entry,
                    unsigned32                            cln,
                    unsigned32                           *ret_val);

int
fat_set_fat_cluster(rtems_filesystem_mount_table_entry_t *mt_entry,
                    unsigned32                            cln,
                    unsigned32                            in_val);

int
fat_scan_fat_for_free_clusters(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                           *chain,
    unsigned32                            count,
    unsigned32                           *cls_added,
    unsigned32                           *last_cl 
);

int
fat_free_fat_clusters_chain(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                            chain
);

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_FAT_FAT_OPERATIONS_H__ */
