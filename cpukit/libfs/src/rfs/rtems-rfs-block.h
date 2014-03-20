/**
 * @file
 *
 * @brief RTEMS File Systems Block Management
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File Systems Block Management.
 *
 * These functions manage the blocks used in the file system.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#if !defined (_RTEMS_RFS_BLOCK_H_)
#define _RTEMS_RFS_BLOCK_H_

#include <rtems/rfs/rtems-rfs-block-pos.h>
#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-file-system.h>

/**
 * Get a block number in the media format and return it in the host format.
 *
 * @param[in] _h is the buffer handle of the block.
 * @param[in] _b is the block number index.
 *
 * @retval block The block number.
 */
#define rtems_rfs_block_get_number(_h, _b) \
  ((rtems_rfs_block_no) \
   (rtems_rfs_read_u32 (rtems_rfs_buffer_data (_h) + \
                        ((_b) * sizeof (rtems_rfs_block_no)))))

/**
 * Set a block number in the media format given a number in the host format.
 *
 * @param[in] _h is the buffer handle of the block.
 * @param[in] _b is the block number index, ie the number of block number not the
 *           buffer offset.
 * @param[in] _n is the block number.
 */
#define rtems_rfs_block_set_number(_h, _b, _n) \
  do { \
    rtems_rfs_write_u32 (rtems_rfs_buffer_data (_h) + \
                         ((_b) * sizeof (rtems_rfs_block_no)), (_n)); \
    rtems_rfs_buffer_mark_dirty (_h); \
  } while (0)

/**
 * A block map manges the block lists that originate from an inode. The inode
 * contains a number of block numbers. A block map takes those block numbers
 * and manages them.
 *
 * The blocks cannot have all ones as a block number nor block 0. The block map
 * is series of block numbers in a blocks. The size of the map determines the
 * way the block numbers are stored. The map uses the following:
 *
 * @li @e Direct Access,
 * @li @e Single Indirect Access, and
 * @li @e Double Indirect Access.
 *
 * Direct access has the blocks numbers in the inode slots. The Single Indirect
 * Access has block numbers in the inode slots that pointer to a table of block
 * numbers that point to data blocks. The Double Indirect Access has block
 * numbers in the inode that point to Single Indirect block tables.
 *
 * The inode can hold a number of Direct, Single Indirect, and Double Indirect
 * block tables. The move from Direct to Single occurs then the block count in
 * the map is above the number of slots in the inode. The move from Single to
 * Double occurs when the map block count is greated than the block numbers per
 * block multipled by the slots in the inode. The move from Single to Double
 * occurs when the map block count is over the block numbers per block squared
 * multipled by the number of slots in the inode.
 *
 * The block map can managed files of the follow size verses block size with 5
 * inode slots:
 *
 *  @li 41,943,040 bytes for a 512 byte block size,
 *  @li 335,544,320 bytes for a 1024 byte block size,
 *  @li 2,684,354,560 bytes for a 2048 byte block size, and
 *  @li 21,474,836,480 bytes for a 4096 byte block size.
 */
typedef struct rtems_rfs_block_map_s
{
  /**
   * Is the map dirty ?
   */
  bool dirty;

  /**
   * The inode this map is attached to.
   */
  rtems_rfs_inode_handle* inode;

  /**
   * The size of the map.
   */
  rtems_rfs_block_size size;

  /**
   * The block map position. Used to navigate the map when seeking. The find
   * call is to a position in the file/directory and is a block number plus
   * offset. The block find only needs to locate a block and not worry about
   * the offset while a seek can be less than a block size yet move across a
   * block boundary. Therefore the position a block map has to maintain must
   * include the offset so seeks work.
   */
  rtems_rfs_block_pos bpos;

  /**
   * The last map block allocated. This is used as the goal when allocating a
   * new map block.
   */
  rtems_rfs_block_no last_map_block;

  /**
   * The last data block allocated. This is used as the goal when allocating a
   * new data block.
   */
  rtems_rfs_block_no last_data_block;

  /**
   * The block map.
   */
  uint32_t blocks[RTEMS_RFS_INODE_BLOCKS];

  /**
   * Singly Buffer handle.
   */
  rtems_rfs_buffer_handle singly_buffer;

  /**
   * Doubly Buffer handle.
   */
  rtems_rfs_buffer_handle doubly_buffer;

} rtems_rfs_block_map;

/**
 * Is the map dirty ?
 */
#define rtems_rfs_block_map_is_dirty(_m) ((_m)->dirty)

/**
 * Return the block count in the map.
 */
#define rtems_rfs_block_map_count(_m) ((_m)->size.count)

/**
 * Return the map's size element.
 */
#define rtems_rfs_block_map_size(_m) (&((_m)->size))

/**
 * Return the size offset for the map.
 */
#define rtems_rfs_block_map_size_offset(_m) ((_m)->size.offset)

/**
 * Are we at the last block in the map ?
 */
#define rtems_rfs_block_map_last(_m) \
  rtems_rfs_block_pos_last_block (&(_m)->bpos, &(_m)->size)

/**
 * Is the position past the end of the block ?
 */
#define rtems_rfs_block_map_past_end(_m, _p) \
  rtems_rfs_block_pos_past_end (_p, &(_m)->size)

/**
 * Return the current position in the map.
 */
#define rtems_rfs_block_map_pos(_f, _m) \
  rtems_rfs_block_get_pos (_f, &(_m)->bpos)

/**
 * Return the map's current block number.
 */
#define rtems_rfs_block_map_block(_m) ((_m)->bpos.bno)

/**
 * Return the map's current block offset.
 */
#define rtems_rfs_block_map_block_offset(_m) ((_m)->bpos.boff)

/**
 * Set the size offset for the map. The map is tagged as dirty.
 *
 * @param[in] map is a pointer to the open map to set the offset in.
 * @param[in] offset is the offset to set in the map's size.
 */
static inline void
rtems_rfs_block_map_set_size_offset (rtems_rfs_block_map* map,
                                     rtems_rfs_block_off  offset)
{
  map->size.offset = offset;
  map->dirty = true;
}

/**
 * Set the map's size. The map is tagged as dirty.
 *
 * @param[in] map is a pointer to the open map to set the offset in.
 * @param[in] size is the size to set in the map's size.
 */
static inline void
rtems_rfs_block_map_set_size (rtems_rfs_block_map*  map,
                              rtems_rfs_block_size* size)
{
  rtems_rfs_block_copy_size (&map->size, size);
  map->dirty = true;
}
/**
 * Open a block map. The block map data in the inode is copied into the
 * map. The buffer handles are opened. The block position is set to the start
 * so a seek of offset 0 will return the first block.
 *
 * @param[in] fs is the file system data.
 * @param[in] inode is a pointer to the inode the map belongs to.
 * @param[in] map is a pointer to the map that is opened.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_open (rtems_rfs_file_system*  fs,
                              rtems_rfs_inode_handle* inode,
                              rtems_rfs_block_map*    map);

/**
 * Close the map. The buffer handles are closed and any help buffers are
 * released.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the map that is opened.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_close (rtems_rfs_file_system* fs,
                               rtems_rfs_block_map*   map);

/**
 * Find a block number in the map from the position provided.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the map to search.
 * @param[in] bpos is a pointer to the block position to find.
 * @param[out] block will contain the block in when found.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_find (rtems_rfs_file_system*  fs,
                              rtems_rfs_block_map*    map,
                              rtems_rfs_block_pos*    bpos,
                              rtems_rfs_buffer_block* block);

/**
 * Seek around the map.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the map to search.
 * @param[in] offset is the distance to seek. It is signed.
 * @param[out] block will contain the block in when found.
 *
 * @retval 0 Successful operation.
 * @retval ENXIO Failed to seek because it is outside the block map.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_seek (rtems_rfs_file_system*  fs,
                              rtems_rfs_block_map*    map,
                              rtems_rfs_pos_rel       offset,
                              rtems_rfs_buffer_block* block);

/**
 * Seek to the next block.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the map to search.
 * @param[out] block will contain the block in when found.
 *
 * @retval 0 Successful operation.
 * @retval ENXIO Failed to seek because it is outside the block map.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_next_block (rtems_rfs_file_system*  fs,
                                    rtems_rfs_block_map*    map,
                                    rtems_rfs_buffer_block* block);

/**
 * Grow the block map by the specified number of blocks.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the open map to grow.
 * @param[in] blocks is the number of blocks to grow the map by.
 * @param[out] new_block will contain first of the blocks allocated
 *                  to the map.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_grow (rtems_rfs_file_system* fs,
                              rtems_rfs_block_map*   map,
                              size_t                 blocks,
                              rtems_rfs_block_no*    new_block);

/**
 * Grow the block map by the specified number of blocks.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the open map to shrink.
 * @param[in] blocks is the number of blocks to shrink the map by. If more
 *               than the number of blocks the map is emptied.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_shrink (rtems_rfs_file_system* fs,
                                rtems_rfs_block_map*   map,
                                size_t                 blocks);

/**
 * Free all blocks in the map.
 *
 * @param[in] fs is the file system data.
 * @param[in] map is a pointer to the open map to free all blocks from.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_block_map_free_all (rtems_rfs_file_system* fs,
                                  rtems_rfs_block_map*   map);

#endif
