/**
 * @file
 *
 * @brief RTEMS File Systems Group Management
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File Systems Group Management.
 *
 * These functions manage the groups used in the file system.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#if !defined (_RTEMS_RFS_GROUP_H_)
#define _RTEMS_RFS_GROUP_H_

/**
 * @ingroup rtems_rfs
 *
 * RTEMS File System Group Management
 */
/**@{*/

#include <rtems/rfs/rtems-rfs-trace.h>
#include <rtems/rfs/rtems-rfs-bitmaps.h>
#include <rtems/rfs/rtems-rfs-buffer.h>

/**
 * Block allocations for a group on disk.
 */
#define RTEMS_RFS_GROUP_BLOCK_BITMAP_BLOCK (0)
#define RTEMS_RFS_GROUP_INODE_BITMAP_BLOCK (1)
#define RTEMS_RFS_GROUP_INODE_BLOCK        (2)

/**
 * @brief Creates bit allocator for blocks in the group simpler.
 *
 * A group is a selection of blocks on the disk. Typically the number of blocks
 * in a group is determined by the number of bits a block holds. This makes the
 * bit allocator for blocks in the group simpler plus is allows a simple way to
 * localise access to files and directories.
 */
typedef struct _rtems_rfs_group
{
  /**
   * Base block number.
   */
  rtems_rfs_buffer_block base;

  /**
   * The number of blocks in the group. Groups may be different sizes.
   */
  size_t size;

  /**
   * The block bitmap control.
   */
  rtems_rfs_bitmap_control block_bitmap;

  /**
   * The handle to the block bitmap buffer.
   */
  rtems_rfs_buffer_handle block_bitmap_buffer;

  /**
   * The inode bitmap control.
   */
  rtems_rfs_bitmap_control inode_bitmap;

  /**
   * The handle to the inode bitmap buffer.
   */
  rtems_rfs_buffer_handle inode_bitmap_buffer;

} rtems_rfs_group;

/**
 * Return the disk's block for a block in a group.
 */
#define rtems_rfs_group_block(_g, _b) (((_g)->base) + (_b))

/**
 * Return the file system inode for a inode in a group.
 */
#define rtems_rfs_group_inode(_f, _g, _i) \
  (((_f)->group_inodes * (_g)) + (_i) + RTEMS_RFS_ROOT_INO)

/**
 * @brief Open a group.
 *
 * Allocate all the resources including the bitmaps.
 *
 * @param fs The file system.
 * @param base The base block number.
 * @param size The number of blocks in the group.
 * @param group Reference to the group to open.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_open (rtems_rfs_file_system* fs,
                          rtems_rfs_buffer_block base,
                          size_t                 size,
                          size_t                 inodes,
                          rtems_rfs_group*       group);

/**
 * @brief Close a group.
 *
 * Release all resources the group holds.
 *
 * @param fs The file system.
 * @param group The group to close.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_close (rtems_rfs_file_system* fs,
                           rtems_rfs_group*       group);

/**
 * @brief Allocate an inode or block.
 *
 * The groups are searched to find the next
 * available inode or block.
 *
 * @param fs The file system data.
 * @param goal The goal to seed the bitmap search.
 * @param inode If true allocate an inode else allocate a block.
 * @param result The allocated bit in the bitmap.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_bitmap_alloc (rtems_rfs_file_system* fs,
                                  rtems_rfs_bitmap_bit   goal,
                                  bool                   inode,
                                  rtems_rfs_bitmap_bit*  result);

/**
 * @brief Free the group allocated bit.
 *
 * @param fs The file system data.
 * @param inode If true the number to free is an inode else it is a block.
 * @param block The inode or block number to free.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_bitmap_free (rtems_rfs_file_system* fs,
                                 bool                   inode,
                                 rtems_rfs_bitmap_bit   no);

/**
 * @brief Test the group allocated bit.
 *
 * @param fs The file system data.
 * @param inode If true the number to free is an inode else it is a block.
 * @param block The inode or block number to free.
 * @param state Return the state of the bit.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_bitmap_test (rtems_rfs_file_system* fs,
                                 bool                   inode,
                                 rtems_rfs_bitmap_bit   no,
                                 bool*                  state);

/**
 * @brief Determine the number of blocks and inodes used.
 *
 * @param fs The file system data.
 * @param blocks The number of blocks used.
 * @param inodes The number of inodes used.
 * @retval int The error number (errno). No error if 0.
 */
int rtems_rfs_group_usage (rtems_rfs_file_system* fs,
                           size_t*                blocks,
                           size_t*                inodes);

/** @} */
#endif
