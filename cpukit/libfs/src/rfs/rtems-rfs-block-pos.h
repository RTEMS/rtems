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
 * RTEMS File Systems Block Position and Size Management.
 *
 * These functions manage the position in a block map as well as a size of data
 * held in a block map. The position is the block count plus the offset into
 * the last block where a block position of 0 and an offset of 0 is the start
 * of a map. The size has a block count plus an offset, but the offset into the
 * last block gives the actual size of the data in the map. This means a size
 * will always have a block count greater than 0 when the file is not empty. A
 * size offset of 0 and a non-zero block count means the length if aligned to
 * the end of the block. For this reason there are 2 similar types so we know
 * which set of rules are in use and the reason for this file.
 */

#if !defined (_RTEMS_RFS_BLOCK_POS_H_)
#define _RTEMS_RFS_BLOCK_POS_H_

#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>

/**
 * The block number is the same type as the inode block number. This makes sure
 * the sizes of the types match.
 */
typedef rtems_rfs_inode_block rtems_rfs_block_no;

/**
 * The offset into a block.
 */
typedef uint32_t rtems_rfs_block_off;

/**
 * A block position is a block number times the block size plus the offset. The
 * block field can be used hold a block number for the position as a look up
 * cache.
 */
typedef struct rtems_rfs_block_pos_s
{
  /**
   * The block index in the map. Range is from 0 to the maps block count minus
   * 1.
   */
  rtems_rfs_block_no bno;

  /**
   * The offset into the block. Must be less than the block size.
   */
  rtems_rfs_block_off boff;

  /**
   * The block number that the bpos + boff map to. The 0 value is invalid and
   * means no block number has been set.
   */
  rtems_rfs_block_no block;

} rtems_rfs_block_pos;

/**
 * Copy a block position.
 *
 * @param _lhs The left hand side.
 * @param _rhs The right hand side.
 */
#define rtems_rfs_block_copy_bpos(_lhs, _rhs) \
  do { (_lhs)->bno = (_rhs)->bno; \
       (_lhs)->boff = (_rhs)->boff; \
       (_lhs)->block = (_rhs)->block; } while (0)

/**
 * Zero a block position.
 *
 * @param bpos A pointer to the block position.
 */
static inline void
rtems_rfs_block_set_bpos_zero (rtems_rfs_block_pos* bpos)
{
  bpos->bno = 0;
  bpos->boff = 0;
  bpos->block = 0;
}

/**
 * Given a position compute the block number and block offset.
 *
 * @param fs The file system data.
 * @param pos The position as an absolute offset from the start.
 * @param bpos Pointer to the block position to fill in.
 */
void rtems_rfs_block_get_bpos (rtems_rfs_file_system*  fs,
                               rtems_rfs_pos           pos,
                               rtems_rfs_block_pos*    bpos);

/**
 * Given a block position compute the absolute offset.
 *
 * @param fs The file system data.
 * @param bpos Pointer to the block position to fill in.
 * @return rtems_rfs_pos The absolute offset.
 */
rtems_rfs_pos rtems_rfs_block_get_pos (rtems_rfs_file_system* fs,
                                       rtems_rfs_block_pos*   bpos);

/**
 * Add the relative position to the block position. The relative position is
 * signed.
 *
 * @param fs The file system data.
 * @param offset The relative offset add to the block position.
 * @param bpos Pointer to the block position to fill in.
 */
static inline void
rtems_rfs_block_add_pos (rtems_rfs_file_system*  fs,
                         rtems_rfs_pos_rel       offset,
                         rtems_rfs_block_pos*    bpos)
{
  rtems_rfs_block_get_bpos (fs,
                            rtems_rfs_block_get_pos (fs, bpos) + offset,
                            bpos);
  bpos->block = 0;
}

/**
 * A block size is the number of blocks less one plus the offset where the
 * offset must be less than the block size.
 */
typedef struct rtems_rfs_block_size_s
{
  /**
   * The count of blocks in a map. A 0 means no blocks and a zero length and
   * the offset should also be 0.
   */
  rtems_rfs_block_no count;

  /**
   * The offset into the block. An offset of 0 means block size, ie the first
   * byte of the next block which is not allocated.
   */
  rtems_rfs_block_off offset;

} rtems_rfs_block_size;

/**
 * Copy a block size.
 *
 * @param _lhs The left hand side.
 * @param _rhs The right hand side.
 */
#define rtems_rfs_block_copy_size(_lhs, _rhs) \
  do { (_lhs)->count = (_rhs)->count; \
       (_lhs)->offset = (_rhs)->offset; } while (0)

/**
 * Last block ?
 */
#define rtems_rfs_block_pos_last_block(_p, _s) \
  ((((_p)->bno == 0) && ((_s)->count == 0)) || ((_p)->bno == ((_s)->count - 1)))

/**
 * Last block ?
 */
#define rtems_rfs_block_pos_past_end(_p, _s) \
  (((_p)->bno && ((_s)->count == 0)) || \
   ((_p)->bno >= (_s)->count) || \
   (((_p)->bno == ((_s)->count - 1)) && ((_p)->boff > (_s)->offset)))

/**
 * Is the block position past the end.
 */
#define rtems_rfs_block_pos_block_past_end(_p, _s) \
  (((_p)->bno && ((_s)->count == 0)) || ((_p)->bno >= (_s)->count))

/**
 * Copy the size to the block position. Note the block position and the size
 * have different block counts.
 */
#define rtems_rfs_block_size_get_bpos(_s, _b) \
  do { (_b)->bno = (_s)->count; \
       (_b)->boff = (_s)->offset; \
       (_b)->block = 0; \
       if ((_b)->boff) --(_b)->bno; } while (0)

/**
 * Do the sizes match ?
 */
#define rtems_rfs_block_size_equal(_lhs, _rhs) \
  (((_lhs)->count == (_rhs)->count) && ((_lhs)->offset == (_rhs)->offset))

/**
 * Zero a block size.
 *
 * @param size A pointer to the block size.
 */
static inline void
rtems_rfs_block_set_size_zero (rtems_rfs_block_size* size)
{
  size->count = 0;
  size->offset = 0;
}

/**
 * Set the size given a position.
 *
 * @param fs The file system data.
 * @param pos The position as an absolute offset from the start.
 * @param size Pointer to the block size to fill in.
 */
void rtems_rfs_block_get_block_size (rtems_rfs_file_system*  fs,
                                     rtems_rfs_pos           pos,
                                     rtems_rfs_block_size*   size);

/**
 * Calculate the position given the number of blocks and the offset. If the
 * block count is 0 the size is 0. If the block is greater than 0 and the
 * offset is 0 the size is number of blocks multipled by the block size and if
 * the offset is not 0 it is the offset into the last block. For example if
 * blocks is 1 and offset is 0 the size is the block size. If the block count
 * is 1 and size is 100 the size is 100.
 *
 * @param fs The file system data.
 * @param size The size in blocks and offset.
 * @return rtems_rfs_pos The size in bytes.
 */
rtems_rfs_pos rtems_rfs_block_get_size (rtems_rfs_file_system* fs,
                                        rtems_rfs_block_size*  size);

#endif
