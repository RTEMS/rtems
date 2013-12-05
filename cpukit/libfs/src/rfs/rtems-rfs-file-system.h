/**
 * @file
 *
 * @brief RTEMS File System Data
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Data
 *
 * This file defines the file system data.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined (_RTEMS_RFS_FILE_SYSTEM_H_)
#define _RTEMS_RFS_FILE_SYSTEM_H_

#include <rtems/rfs/rtems-rfs-group.h>

/**
 * Superblock offsets and values.
 */
#define RTEMS_RFS_SB_OFFSET_MAGIC           (0)
#define RTEMS_RFS_SB_MAGIC                  (0x28092001)
#define RTEMS_RFS_SB_OFFSET_VERSION         (RTEMS_RFS_SB_OFFSET_MAGIC           + 4)
#define RTEMS_RFS_SB_OFFSET_BLOCK_SIZE      (RTEMS_RFS_SB_OFFSET_VERSION         + 4)
#define RTEMS_RFS_SB_OFFSET_BLOCKS          (RTEMS_RFS_SB_OFFSET_BLOCK_SIZE      + 4)
#define RTEMS_RFS_SB_OFFSET_BAD_BLOCKS      (RTEMS_RFS_SB_OFFSET_BLOCKS          + 4)
#define RTEMS_RFS_SB_OFFSET_MAX_NAME_LENGTH (RTEMS_RFS_SB_OFFSET_BAD_BLOCKS      + 4)
#define RTEMS_RFS_SB_OFFSET_GROUPS          (RTEMS_RFS_SB_OFFSET_MAX_NAME_LENGTH + 4)
#define RTEMS_RFS_SB_OFFSET_GROUP_BLOCKS    (RTEMS_RFS_SB_OFFSET_GROUPS          + 4)
#define RTEMS_RFS_SB_OFFSET_GROUP_INODES    (RTEMS_RFS_SB_OFFSET_GROUP_BLOCKS    + 4)
#define RTEMS_RFS_SB_OFFSET_INODE_SIZE      (RTEMS_RFS_SB_OFFSET_GROUP_INODES    + 4)

/**
 * RFS Version Number.
 */
#define RTEMS_RFS_VERSION (0x00000000)

/**
 * RFS Version Number Mask. The mask determines which bits of the version
 * number indicate compatility issues.
 */
#define RTEMS_RFS_VERSION_MASK INT32_C(0x00000000)

/**
 * The root inode number. Do not use 0 as this has special meaning in some
 * Unix operating systems.
 */
#define RTEMS_RFS_ROOT_INO (1)

/**
 * Empty inode number.
 */
#define RTEMS_RFS_EMPTY_INO (0)

/**
 * The number of blocks in the inode. This number effects the size of the
 * inode and that effects the overhead of the inode tables in a group.
 */
#define RTEMS_RFS_INODE_BLOCKS (5)

/**
 * The inode overhead is the percentage of space reserved for inodes. It is
 * calculated as the percentage number of blocks in a group. The number of
 * blocks in a group is the number of bits a block can hold.
 */
#define RTEMS_RFS_INODE_OVERHEAD_PERCENTAGE (1)

/**
 * Number of blocks in the superblock. Yes I know it is a superblock and not
 * superblocks but if for any reason this needs to change it is handled.
 */
#define RTEMS_RFS_SUPERBLOCK_SIZE (1)

/**
 * The maximum number of buffers held by the file system at any one time.
 */
#define RTEMS_RFS_FS_MAX_HELD_BUFFERS (5)

/**
 * Absolute position. Make a 64bit value.
 */
typedef uint64_t rtems_rfs_pos;

/**
 * Relative position. Make a 64bit value.
 */
typedef int64_t rtems_rfs_pos_rel;

/**
 * Flags to control the file system.
 */
#define RTEMS_RFS_FS_BITMAPS_HOLD      (1 << 0) /**< Do not release bitmaps
                                                 * when finished. Default is
                                                 * off so they are released. */
#define RTEMS_RFS_FS_NO_LOCAL_CACHE    (1 << 1) /**< Do not cache the buffers
                                                 * and release directly to the
                                                 * buffer support layer. The
                                                 * default is to hold buffers. */
#define RTEMS_RFS_FS_FORCE_OPEN        (1 << 2) /**< Force open and ignore any
                                                 * errors. */
#define RTEMS_RFS_FS_READ_ONLY         (1 << 3) /**< Make the mount
                                                 * read-only. Currently not
                                                 * supported. */
/**
 * RFS File System data.
 */
struct _rtems_rfs_file_system
{
  /**
   * Flags to control the file system. Some can be controlled via the ioctl.
   */
  uint32_t flags;

  /**
   * The number of blocks in the disk. The size of the disk is the number of
   * blocks by the block size. This should be within a block size of the size
   * returned by the media driver.
   */
  size_t blocks;

  /**
   * The size of a block. This must be a multiple of the disk's media block
   * size.
   */
  size_t block_size;

  /**
   * The file descriptor for device I/O.
   */
  int device;

#if RTEMS_RFS_USE_LIBBLOCK
  /**
   * The disk device. This is the data about the block device this file system
   * is mounted on. We access the data held in this structure rather than
   * making an extra copy in this structure.
   */
  rtems_disk_device* disk;
#else
  /**
   * The number of blocks in the file system.
   */
  size_t size;
#endif

  /**
   * Inode count.
   */
  uint32_t inodes;

  /**
   * Bad block blocks. This is a table of blocks that have been found to be
   * bad.
   */
  uint32_t bad_blocks;

  /**
   * Maximum length of names supported by this file system.
   */
  uint32_t max_name_length;

  /**
   * A disk is broken down into a series of groups.
   */
  rtems_rfs_group* groups;

  /**
   * Number of groups.
   */
  int group_count;

  /**
   * Number of blocks in a group.
   */
  size_t group_blocks;

  /**
   * Number of inodes in a group.
   */
  size_t group_inodes;

  /**
   * Number of inodes in each block.
   */
  size_t inodes_per_block;

  /**
   * Number of block numbers in a block.
   */
  size_t blocks_per_block;

  /**
   * Block map single indirect count. This is the block number per block
   * multiplied but the slots in the inode.
   */
  size_t block_map_singly_blocks;

  /**
   * Block map double indirect count. This is the block number per block
   * squared and multiplied by the slots in the inode. It is the maximum
   * number of blocks a map (file/directory) can have.
   */
  size_t block_map_doubly_blocks;

  /**
   * Number of buffers held before releasing back to the cache.
   */
  uint32_t max_held_buffers;

  /**
   * List of buffers attached to buffer handles. Allows sharing.
   */
  rtems_chain_control buffers;

  /**
   * Number of buffers held on the buffers list.
   */
  uint32_t buffers_count;

  /**
   * List of buffers that need to be released when the processing of a file
   * system request has completed.
   */
  rtems_chain_control release;

  /**
   * Number of buffers held on the release list.
   */
  uint32_t release_count;

  /**
   * List of buffers that need to be released modified when the processing of a
   * file system request has completed.
   */
  rtems_chain_control release_modified;

  /**
   * Number of buffers held on the release modified list.
   */
  uint32_t release_modified_count;

  /**
   * List of open shared file node data. The shared node data such as the inode
   * and block map allows a single file to be open more than once.
   */
  rtems_chain_control file_shares;

  /**
   * Pointer to user data supplied when opening.
   */
  void* user;
};

/**
 * Return the flags.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_flags(_f) ((_f)->flags)
/**
 * Should bitmap buffers be released when finished ?
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_release_bitmaps(_f) (!((_f)->flags & RTEMS_RFS_FS_BITMAPS_HOLD))

/**
 * Are the buffers locally cache or released back to the buffering layer ?
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_no_local_cache(_f) ((_f)->flags & RTEMS_RFS_FS_NO_LOCAL_CACHE)

/**
 * The disk device number.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#if RTEMS_RFS_USE_LIBBLOCK
#define rtems_rfs_fs_device(_fs) ((_fs)->disk)
#else
#define rtems_rfs_fs_device(_fs) ((_fs)->device)
#endif

/**
 * The size of the disk in blocks.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_blocks(_fs) ((_fs)->blocks)

/**
 * The block size.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_block_size(_fs) ((_fs)->block_size)

/**
 * The number of inodes.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#define rtems_rfs_fs_inodes(_fs) ((_fs)->inodes)

/**
 * Calculate a block in the file system given the group and the block within
 * the group.
 *
 * @param[in] _fs is a pointer to the file system.
 * @param[in] _grp is the group.
 * @param[in] _blk is the block within the group.
 * @return The absolute block number.
 */
#define rtems_rfs_fs_block(_fs, _grp, _blk) \
  ((((_fs)->group_blocks) * (_grp)) + (_blk) + 1)

/**
 * The media size of the disk in media size blocks.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#if RTEMS_RFS_USE_LIBBLOCK
#define rtems_rfs_fs_media_blocks(_fs) ((_fs)->disk->size)
#else
#define rtems_rfs_fs_media_blocks(_fs) ((_fs)->media_size)
#endif

/**
 * The media block size. This is the size of a block on disk. For a device I/O
 * this value is 1.
 *
 * @param[in] _fs is a pointer to the file system.
 */
#if RTEMS_RFS_USE_LIBBLOCK
#define rtems_rfs_fs_media_block_size(_fs) ((_fs)->disk->media_block_size)
#else
#define rtems_rfs_fs_media_block_size(_fs) (1)
#endif

/**
 * The maximum length of a name supported by the file system.
 */
#define rtems_rfs_fs_max_name(_fs) ((_fs)->max_name_length)

/**
 * Return the maximum number of blocks in a block map.
 *
 * @return uint32_t The maximum number of blocks possible.
 */
#define rtems_rfs_fs_max_block_map_blocks(_fs) ((_fs)->block_map_doubly_blocks)

/**
 * Return the user pointer.
 */
#define rtems_rfs_fs_user(_fs) ((_fs)->user)

/**
 * Return the size of the disk in bytes.
 *
 * @param[in] fs is a pointer to the file system.
 * @return uint64_t The size of the disk in bytes.
 */
uint64_t rtems_rfs_fs_size(rtems_rfs_file_system* fs);

/**
 * The size of the disk in bytes calculated from the media parameters..
 *
 * @param[in] fs is a pointer to the file system.
 * @return uint64_t The size of the disk in bytes.
 */
uint64_t rtems_rfs_fs_media_size (rtems_rfs_file_system* fs);

/**
 * Open the file system given a file path.
 *
 * @param[in] name is a pointer to the device to open.
 * @param[in] fs is the file system data filled in by this call.
 * @param[in] user is a pointer to the user data.
 * @param[in] flags is a initial set of user flags for the file system.
 * @param[in] max_held_buffers is the maximum number of buffers the RFS holds.
 *
 * @retval 0 Successful operation.
 * @retval -1 Error. See errno
 */
int rtems_rfs_fs_open (const char*             name,
                       void*                   user,
                       uint32_t                flags,
                       uint32_t                max_held_buffers,
                       rtems_rfs_file_system** fs);

/**
 * Close the file system.
 *
 * @param[in] fs is the file system data.
 *
 * @retval 0 Successful operation.
 * @retval -1 Error. See errno
 */
int rtems_rfs_fs_close (rtems_rfs_file_system* fs);

#endif
