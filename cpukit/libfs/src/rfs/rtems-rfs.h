/**
 * @file
 *
 * @brief RFS File system Initialization
 * @ingroup rtems_rfs
 *
 * RTEMS File System
 *
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined(RTEMS_RFS_DEFINED)
#define RTEMS_RFS_DEFINED

#include <rtems.h>
#include <rtems/fs.h>

/**
 * @defgroup rtems_rfs RTEMS File System Group Management
 *
 * @ingroup FileSystemTypesAndMount
 */
/**@{*/

/**
 * Initialise the RFS File system.
 */
int rtems_rfs_rtems_initialise (rtems_filesystem_mount_table_entry_t *mt_entry, const void *data);

/**@}*/
#endif
