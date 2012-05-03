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
 * RTEMS File System Format.
 *
 * This function lets you format a disk in the RFS format.
 */

#if !defined (_RTEMS_RFS_FORMAT_H_)
#define _RTEMS_RFS_FORMAT_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rfs/rtems-rfs-trace.h>

/**
 * RFS File System Configuration data used to format the file system. For
 * default values leave the field set to 0.
 */
typedef struct _rtems_rfs_format_config
{
  /**
   * The size of a block.
   */
  size_t block_size;

  /**
   * The number of blocks in a group.
   */
  size_t group_blocks;

  /**
   * The number of inodes in a group.
   */
  size_t group_inodes;

  /**
   * The percentage overhead allocated to inodes.
   */
  int inode_overhead;

  /**
   * The maximum length of a name.
   */
  size_t max_name_length;

  /**
   * Initialise the inode tables to all ones.
   */
  bool initialise_inodes;

  /**
   * Is the format verbose.
   */
  bool verbose;

} rtems_rfs_format_config;

/**
 * RFS Format command.
 *
 * @param name The device name to format.
 * @param config Pointer to a configuration table.
 * @retval -1 Error. See errno.
 * @retval 0 No error. Format successful.
 */
int rtems_rfs_format (const char* name, const rtems_rfs_format_config* config);

#ifdef __cplusplus
}
#endif

#endif
