/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File System
 *
 */

#if !defined(RTEMS_RFS_DEFINED)
#define RTEMS_RFS_DEFINED

#include <rtems.h>
#include <rtems/fs.h>

/**
 * Initialise the RFS File system.
 */
int rtems_rfs_rtems_initialise (rtems_filesystem_mount_table_entry_t *mt_entry);

#endif
