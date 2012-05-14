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
 * RTEMS File System File Support
 *
 * This file provides the support functions.
 */

#if !defined (_RTEMS_RFS_FILE_H_)
#define _RTEMS_RFS_FILE_H_

#include <rtems/libio_.h>

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>

/**
 * File data that is shared by various file handles accessing the same file. We
 * hold various inode values common to the file that can change frequently so
 * the inode is not thrashed yet we meet the requirements of the POSIX
 * standard. The stat call needs to check the shared file data.
 */
typedef struct _rtems_rfs_file_shared
{
  /**
   * The shared parts are maintained as a list.
   */
  rtems_chain_node link;

  /**
   * Reference count the users of this data.
   */
  int references;

  /**
   * The inode for the file.
   */
  rtems_rfs_inode_handle inode;

  /**
   * The block map for the file. The handle holds the file's position not the
   * map.
   */
  rtems_rfs_block_map map;

  /**
   * The size of the file as taken from the inode. The map's size and
   * this size should be the same.
   */
  rtems_rfs_block_size size;

  /**
   * The access time. The last time the file was read.
   */
  rtems_rfs_time atime;

  /**
   * The modified time. The last time the file was written too.
   */
  rtems_rfs_time mtime;

  /**
   * The change time. The last time the inode was written too.
   */
  rtems_rfs_time ctime;

  /**
   * Hold a pointer to the file system data so users can take the handle and
   * use it without the needing to hold the file system data pointer.
   */
  rtems_rfs_file_system* fs;

} rtems_rfs_file_shared;

/**
 * Get the atime.
 *
 * @param shared The shared file data.
 * @return rtems_rfs_time The atime.
 */
static inline rtems_rfs_time
rtems_rfs_file_shared_get_atime (rtems_rfs_file_shared* shared)
{
  return shared->atime;
}

/**
 * Get the mtime.
 *
 * @param shared The shared file data.
 * @return rtems_rfs_time The mtime.
 */
static inline rtems_rfs_time
rtems_rfs_file_shared_get_mtime (rtems_rfs_file_shared* shared)
{
  return shared->mtime;
}

/**
 * Get the ctime.
 *
 * @param shared The shared file data.
 * @return rtems_rfs_time The ctime.
 */
static inline rtems_rfs_time
rtems_rfs_file_shared_get_ctime (rtems_rfs_file_shared* shared)
{
  return shared->ctime;
}

/**
 * Get the block count.
 *
 * @param shared The shared file data.
 * @return uint32_t The block count.
 */
static inline uint32_t
rtems_rfs_file_shared_get_block_count (rtems_rfs_file_shared* shared)
{
  return shared->size.count;
}

/**
 * Get the block offset.
 *
 * @param shared The shared file data.
 * @return uint16_t The block offset.
 */
static inline uint16_t
rtems_rfs_file_shared_get_block_offset (rtems_rfs_file_shared* shared)
{
  return shared->size.offset;
}

/**
 * Calculate the size of data.
 *
 * @param fs The file system data.
 * @oaram shared The shared file data.
 * @return rtems_rfs_pos The data size in bytes.
 */
static inline rtems_rfs_pos
rtems_rfs_file_shared_get_size (rtems_rfs_file_system* fs,
                                rtems_rfs_file_shared* shared)
{
  return rtems_rfs_block_get_size (fs, &shared->size);
}

/**
 * File flags.
 */
#define RTEMS_RFS_FILE_NO_ATIME_UPDATE  (1 << 0) /**< Do not update the atime
                                                  * field in the inode if
                                                  * set. */
#define RTEMS_RFS_FILE_NO_MTIME_UPDATE  (1 << 1) /**< Do not update the mtime
                                                  * field in the inode if
                                                  * set. */
#define RTEMS_RFS_FILE_NO_LENGTH_UPDATE (1 << 2) /**< Do not update the position
                                                  * field in the inode if
                                                  * set. */

/**
 * File data used to managed an open file.
 */
typedef struct _rtems_rfs_file_handle
{
  /**
   * Special flags that can be controlled by the fctrl call.
   */
  int flags;

  /**
   * The buffer of data at the file's position.
   */
  rtems_rfs_buffer_handle buffer;

  /**
   * The block position of this file handle.
   */
  rtems_rfs_block_pos bpos;

  /**
   * Pointer to the shared file data.
   */
  rtems_rfs_file_shared* shared;

} rtems_rfs_file_handle;

/**
 * Access the data in the buffer.
 */
#define rtems_rfs_file_data(_f) \
  (rtems_rfs_buffer_data (&(_f)->buffer) + (_f)->bpos.boff)

/**
 * Return the file system data pointer given a file handle.
 */
#define rtems_rfs_file_fs(_f) ((_f)->shared->fs)

/**
 * Return the file's inode handle pointer given a file handle.
 */
#define rtems_rfs_file_inode(_f) (&(_f)->shared->inode)

/**
 * Return the file's block map pointer given a file handle.
 */
#define rtems_rfs_file_map(_f) (&(_f)->shared->map)

/**
 * Return the file's block position pointer given a file handle.
 */
#define rtems_rfs_file_bpos(_f) (&(_f)->bpos)

/**
 * Return the file's block number given a file handle.
 */
#define rtems_rfs_file_block(_f) ((_f)->bpos.bno)

/**
 * Return the file's block offset given a file handle.
 */
#define rtems_rfs_file_block_offset(_f) ((_f)->bpos.boff)

/**
 * Set the file's block position given a file position (absolute).
 */
#define rtems_rfs_file_set_bpos(_f, _p) \
  rtems_rfs_block_get_bpos (rtems_rfs_file_fs (_f), _p, (&(_f)->bpos))

/**
 * Return the file's buffer handle pointer given a file handle.
 */
#define rtems_rfs_file_buffer(_f) (&(_f)->buffer)

/**
 * Update the access time field of the inode when reading if flagged to do so.
 */
#define rtems_rfs_file_update_atime(_f) \
  (((_f)->flags & RTEMS_RFS_FILE_NO_ATIME_UPDATE) == 0)

/**
 * Update the modified time field of the inode when writing if flagged to do so.
 */
#define rtems_rfs_file_update_mtime(_f) \
  (((_f)->flags & RTEMS_RFS_FILE_NO_MTIME_UPDATE) == 0)

/**
 * Update the length field of the inode.
 */
#define rtems_rfs_file_update_length(_f) \
  (((_f)->flags & RTEMS_RFS_FILE_NO_LENGTH_UPDATE) == 0)

/**
 * Return the shared size varable.
 */
#define rtems_rfs_file_get_size(_f) \
   (&(_f)->shared->size)

/**
 * Return the size of file.
 */
#define rtems_rfs_file_size(_f) \
  rtems_rfs_file_shared_get_size (rtems_rfs_file_fs (_f), (_f)->shared)

/**
 * Return the file block count.
 */
#define rtems_rfs_file_size_count(_f) \
  rtems_rfs_file_shared_get_block_count ((_f)->shared)

/**
 * Return the file block offset.
 */
#define rtems_rfs_file_size_offset(_f) \
  rtems_rfs_file_shared_get_block_offset ((_f)->shared)

/**
 * Open a file handle.
 *
 * @param fs The file system.
 * @param ino The inode number of the file to be opened.
 * @param handle Return the handle pointer in this handle.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_open (rtems_rfs_file_system*  fs,
                         rtems_rfs_ino           ino,
                         int                     oflag,
                         rtems_rfs_file_handle** handle);

/**
 * Close an open file handle.
 *
 * @param fs The file system.
 * @param handle The open file handle.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_close (rtems_rfs_file_system* fs,
                          rtems_rfs_file_handle* handle);

/**
 * Start I/O on a block of a file. This call only requests the block from the
 * media if reading and makes the buffer available to you the via the
 * rtems_rfs_file_data interface after the call. The available amount data is
 * taken from the current file position until the end of the block. The file
 * position is not adujsted until the I/O ends. An I/O request cannot perform
 * I/O past the end of a block so the call returns the amount of data
 * available.
 *
 * @param handle The file handle.
 * @param available The amount of data available for I/O.
 * @param read The I/O operation is a read so the block is read from the media.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_io_start (rtems_rfs_file_handle* handle,
                             size_t*                available,
                             bool                   read);

/**
 * End the I/O. Any buffers held in the file handle and returned to the
 * cache. If inode updating is not disable and the I/O is a read the atime
 * field is updated and if a write I/O the mtime is updated.
 *
 * If the file's position is updated by the size amount.
 *
 * @param handle The file handle.
 * @param size The amount of data read or written.
 * @param read The I/O was a read if true else it was a write.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_io_end (rtems_rfs_file_handle* handle,
                           size_t                 size,
                           bool                   read);

/**
 * Release the I/O resources without any changes. If data has changed in the
 * buffer and the buffer was not already released as modified the data will be
 * lost.
 *
 * @param handle The file handle.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_io_release (rtems_rfs_file_handle* handle);

/**
 * The file to the position returning the old position. The position is
 * abolute.
 *
 * @param handle The file handle.
 * @param pos The position to seek to.
 * @param new_pos The actual position.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_seek (rtems_rfs_file_handle* handle,
                         rtems_rfs_pos          pos,
                         rtems_rfs_pos*         new_pos);

/**
 * Set the size of the file to the new size. This can extend the file to a new
 * size.
 *
 * @param handle The file handle.
 * @param size The new size of the file.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_file_set_size (rtems_rfs_file_handle* handle,
                             rtems_rfs_pos          size);

/**
 * Return the shared file data for an ino.
 *
 * @param fs The file system data.
 * @param ino The inode number to locate the data for.
 * @return rtems_rfs_file_shared* The shared data or NULL is not located.
 */
rtems_rfs_file_shared* rtems_rfs_file_get_shared (rtems_rfs_file_system* fs,
                                                  rtems_rfs_ino          ino);


#endif
