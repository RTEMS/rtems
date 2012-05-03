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
 * RTEMS File Systems Block Routines.
 *
 * These functions manage blocks in the RFS file system. A block is an area of
 * the media and its size is set for a each specific media. The block size is
 * set when the file system is set up and needs to be matched for it to be read
 * correctly.
 *
 * Blocks are managed as groups. A block group or "group" is part of the total
 * number of blocks being managed by the file system and exist to allow
 * resources to localised. A file in a directory will be allocated blocks in
 * the same group as the directory, and the blocks for the file will also be
 * allocated in the same group.
 *
 * A group consist of a block bitmap, inodes and data blocks. The first block
 * of the file system will hold the superblock. The block bitmap is a
 * collection of blocks that hold a map of bits, one bit per block for each
 * block in the group. When a file system is mounted the block bitmaps are read
 * and a summary bit map is made. The summary bitmap has a single bit for 32
 * bits in the bitmap and is set when all 32 bits it maps to are set. This
 * speeds up the search for a free block by a factor of 32.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-group.h>
#include <rtems/rfs/rtems-rfs-inode.h>

void
rtems_rfs_block_get_bpos (rtems_rfs_file_system* fs,
                          rtems_rfs_pos          pos,
                          rtems_rfs_block_pos*   bpos)
{
  bpos->bno  = pos / rtems_rfs_fs_block_size (fs);
  bpos->boff = pos % rtems_rfs_fs_block_size (fs);
}

rtems_rfs_pos
rtems_rfs_block_get_pos (rtems_rfs_file_system* fs,
                         rtems_rfs_block_pos*   bpos)
{
  return (bpos->bno * rtems_rfs_fs_block_size (fs)) + bpos->boff;
}

void
rtems_rfs_block_get_block_size (rtems_rfs_file_system* fs,
                                rtems_rfs_pos          pos,
                                rtems_rfs_block_size*  size)
{
  if (pos == 0)
    rtems_rfs_block_set_size_zero (size);
  else
  {
    size->count  = pos / rtems_rfs_fs_block_size (fs) + 1;
    size->offset = pos % rtems_rfs_fs_block_size (fs);
  }
}

rtems_rfs_pos
rtems_rfs_block_get_size (rtems_rfs_file_system* fs,
                          rtems_rfs_block_size*  size)
{
  uint32_t offset;
  uint64_t block_size;
  if (size->count == 0)
    return 0;
  if (size->offset == 0)
    offset = rtems_rfs_fs_block_size (fs);
  else
    offset = size->offset;
  block_size = rtems_rfs_fs_block_size (fs);
  return (((uint64_t) (size->count - 1)) * block_size) + offset;
}

int
rtems_rfs_block_map_open (rtems_rfs_file_system*  fs,
                          rtems_rfs_inode_handle* inode,
                          rtems_rfs_block_map*    map)
{
  int b;
  int rc;

  /*
   * Set the count to 0 so at least find fails, then open the handle and make
   * sure the inode has been loaded into memory. If we did not load the inode
   * do not unload it. The caller may assume it is still loaded when we return.
   */

  map->dirty = false;
  map->inode = NULL;
  rtems_rfs_block_set_size_zero (&map->size);
  rtems_rfs_block_set_bpos_zero (&map->bpos);

  rc = rtems_rfs_buffer_handle_open (fs, &map->singly_buffer);
  if (rc > 0)
    return rc;
  rc = rtems_rfs_buffer_handle_open (fs, &map->doubly_buffer);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_inode_load (fs, inode);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &map->singly_buffer);
    rtems_rfs_buffer_handle_close (fs, &map->doubly_buffer);
    return rc;
  }

  /*
   * Extract the block and block count data from the inode into the targets
   * byte order.
   */
  map->inode = inode;
  for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
    map->blocks[b] = rtems_rfs_inode_get_block (inode, b);
  map->size.count = rtems_rfs_inode_get_block_count (inode);
  map->size.offset = rtems_rfs_inode_get_block_offset (inode);
  map->last_map_block = rtems_rfs_inode_get_last_map_block (inode);
  map->last_data_block = rtems_rfs_inode_get_last_data_block (inode);

  rc = rtems_rfs_inode_unload (fs, inode, false);

  return rc;
}

int
rtems_rfs_block_map_close (rtems_rfs_file_system* fs,
                           rtems_rfs_block_map*   map)
{
  int rc = 0;
  int brc;

  if (map->dirty && map->inode)
  {
    brc = rtems_rfs_inode_load (fs, map->inode);
    if (brc > 0)
      rc = brc;

    if (rc == 0)
    {
      int b;

      for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
        rtems_rfs_inode_set_block (map->inode, b, map->blocks[b]);
      rtems_rfs_inode_set_block_count (map->inode, map->size.count);
      rtems_rfs_inode_set_block_offset (map->inode, map->size.offset);
      rtems_rfs_inode_set_last_map_block (map->inode, map->last_map_block);
      rtems_rfs_inode_set_last_data_block (map->inode, map->last_data_block);

      brc = rtems_rfs_inode_unload (fs, map->inode, true);
      if (brc > 0)
        rc = brc;

      map->dirty = false;
    }
  }

  map->inode = NULL;

  brc = rtems_rfs_buffer_handle_close (fs, &map->singly_buffer);
  if ((brc > 0) && (rc == 0))
    rc = brc;
  brc = rtems_rfs_buffer_handle_close (fs, &map->doubly_buffer);
  if ((brc > 0) && (rc == 0))
    rc = brc;
  return rc;
}

/**
 * Find a block indirectly held in a table of block numbers.
 *
 * @param fs The file system.
 * @param buffer The handle to access the block data by.
 * @param block The block number of the table of block numbers.
 * @param offset The offset in the table of the block number to return. This is
 *               a block number offset not a byte offset into the table.
 * @param result Pointer to the result of the search.
 * @return int The error number (errno). No error if 0.
 */
static int
rtems_rfs_block_find_indirect (rtems_rfs_file_system*   fs,
                               rtems_rfs_buffer_handle* buffer,
                               rtems_rfs_block_no       block,
                               int                      offset,
                               rtems_rfs_block_no*      result)
{
   int rc;

  /*
   * If the handle has a buffer and this request is a different block the current
   * buffer is released.
   */
  rc = rtems_rfs_buffer_handle_request (fs, buffer, block, true);
  if (rc > 0)
    return rc;

  *result = rtems_rfs_block_get_number (buffer, offset);
  if ((*result + 1) == 0)
    *result = 0;

  if (*result >= rtems_rfs_fs_blocks (fs))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BLOCK_FIND))
      printf ("rtems-rfs: block-find: invalid block in table:"
              " block=%" PRId32 ", indirect=%" PRId32 "/%d\n", *result, block, offset);
    *result = 0;
    rc = EIO;
  }

  return 0;
}

int
rtems_rfs_block_map_find (rtems_rfs_file_system* fs,
                          rtems_rfs_block_map*   map,
                          rtems_rfs_block_pos*   bpos,
                          rtems_rfs_block_no*    block)
{
  int rc = 0;

  *block = 0;

  /*
   * Range checking here makes the remaining logic simpler.
   */
  if (rtems_rfs_block_pos_block_past_end (bpos, &map->size))
    return ENXIO;

  /*
   * If the block position is the same and we have found the block just return it.
   */
  if ((bpos->bno == map->bpos.bno) && (map->bpos.block != 0))
  {
    *block = map->bpos.block;
  }
  else
  {
    /*
     * Determine the type of access we need to perform. If the number of blocks
     * is less than or equal to the number of slots in the inode the blocks are
     * directly accessed.
     */
    if (map->size.count <= RTEMS_RFS_INODE_BLOCKS)
    {
      *block = map->blocks[bpos->bno];
    }
    else
    {
      /*
       * The map is either singly or doubly indirect.
       */
      rtems_rfs_block_no direct;
      rtems_rfs_block_no singly;

      direct = bpos->bno % fs->blocks_per_block;
      singly = bpos->bno / fs->blocks_per_block;

      if (map->size.count <= fs->block_map_singly_blocks)
      {
        /*
         * This is a single indirect table of blocks anchored off a slot in the
         * inode.
         */
        rc = rtems_rfs_block_find_indirect (fs,
                                            &map->singly_buffer,
                                            map->blocks[singly],
                                            direct, block);
      }
      else
      {
        /*
         * The map is doubly indirect.
         */
        rtems_rfs_block_no doubly;

        doubly  = singly / fs->blocks_per_block;
        singly %= fs->blocks_per_block;

        if (map->size.count < fs->block_map_doubly_blocks)
        {
          rc = rtems_rfs_block_find_indirect (fs,
                                              &map->doubly_buffer,
                                              map->blocks[doubly],
                                              singly, &singly);
          if (rc == 0)
          {
            rc = rtems_rfs_block_find_indirect (fs,
                                                &map->singly_buffer,
                                                singly, direct, block);
          }
        }
        else
        {
          /*
           * This should never happen. Here so Joel can remove once his coverage
           * testing gets to the file systems.
           */
          rc = ENXIO;
        }
      }
    }
  }

  if (rc == 0)
  {
    rtems_rfs_block_copy_bpos (&map->bpos, bpos);
    map->bpos.block = *block;
  }

  return rc;
}

int
rtems_rfs_block_map_seek (rtems_rfs_file_system* fs,
                          rtems_rfs_block_map*   map,
                          rtems_rfs_pos_rel      offset,
                          rtems_rfs_block_no*    block)
{
  rtems_rfs_block_pos bpos;
  rtems_rfs_block_copy_bpos (&bpos, &map->bpos);
  rtems_rfs_block_add_pos (fs, offset, &bpos);
  return rtems_rfs_block_map_find (fs, map, &bpos, block);
}

int
rtems_rfs_block_map_next_block (rtems_rfs_file_system* fs,
                                rtems_rfs_block_map*   map,
                                rtems_rfs_block_no*    block)
{
  rtems_rfs_block_pos bpos;
  bpos.bno = map->bpos.bno + 1;
  bpos.boff = 0;
  bpos.block = 0;
  return rtems_rfs_block_map_find (fs, map, &bpos, block);
}

/**
 * Allocate an indirect block to a map.
 *
 * @param fs The file system data.
 * @param map The map the allocation is for.
 * @param buffer The buffer the indirect block is accessed by.
 * @param block The block number of the indirect block allocated.
 * @param upping True is upping the map to the next indirect level.
 * @return int The error number (errno). No error if 0.
 */
static int
rtems_rfs_block_map_indirect_alloc (rtems_rfs_file_system*   fs,
                                    rtems_rfs_block_map*     map,
                                    rtems_rfs_buffer_handle* buffer,
                                    rtems_rfs_block_no*      block,
                                    bool                     upping)
{
  rtems_rfs_bitmap_bit new_block;
  int                  rc;
  /*
   * Save the new block locally because upping can have *block pointing to the
   * slots which are cleared when upping.
   */
  rc = rtems_rfs_group_bitmap_alloc (fs, map->last_map_block, false, &new_block);
  if (rc > 0)
    return rc;
  rc = rtems_rfs_buffer_handle_request (fs, buffer, new_block, false);
  if (rc > 0)
  {
    rtems_rfs_group_bitmap_free (fs, false, new_block);
    return rc;
  }
  memset (rtems_rfs_buffer_data (buffer), 0xff, rtems_rfs_fs_block_size (fs));
  if (upping)
  {
    int b;
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BLOCK_MAP_GROW))
      printf ("rtems-rfs: block-map-grow: upping: block-count=%" PRId32 "\n",
              map->size.count);
    for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
      rtems_rfs_block_set_number (buffer, b, map->blocks[b]);
    memset (map->blocks, 0, sizeof (map->blocks));
  }
  rtems_rfs_buffer_mark_dirty (buffer);
  *block = new_block;
  map->last_map_block = new_block;
  return 0;
}

int
rtems_rfs_block_map_grow (rtems_rfs_file_system* fs,
                          rtems_rfs_block_map*   map,
                          size_t                 blocks,
                          rtems_rfs_block_no*    new_block)
{
  int b;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BLOCK_MAP_GROW))
    printf ("rtems-rfs: block-map-grow: entry: blocks=%zd count=%" PRIu32 "\n",
            blocks, map->size.count);

  if ((map->size.count + blocks) >= rtems_rfs_fs_max_block_map_blocks (fs))
    return EFBIG;

  /*
   * Allocate a block at a time. The buffer handles hold the blocks so adding
   * this way does not thrash the cache with lots of requests.
   */
  for (b = 0; b < blocks; b++)
  {
    rtems_rfs_bitmap_bit block;
    int                  rc;

    /*
     * Allocate the block. If an indirect block is needed and cannot be
     * allocated free this block.
     */

    rc = rtems_rfs_group_bitmap_alloc (fs, map->last_data_block,
                                       false, &block);
    if (rc > 0)
      return rc;

    if (map->size.count < RTEMS_RFS_INODE_BLOCKS)
      map->blocks[map->size.count] = block;
    else
    {
      /*
       * Single indirect access is occuring. It could still be doubly indirect.
       */
      rtems_rfs_block_no direct;
      rtems_rfs_block_no singly;

      direct = map->size.count % fs->blocks_per_block;
      singly = map->size.count / fs->blocks_per_block;

      if (map->size.count < fs->block_map_singly_blocks)
      {
        /*
         * Singly indirect tables are being used. Allocate a new block for a
         * mapping table if direct is 0 or we are moving up (upping). If upping
         * move the direct blocks into the table and if not this is the first
         * entry of a new block.
         */
        if ((direct == 0) ||
            ((singly == 0) && (direct == RTEMS_RFS_INODE_BLOCKS)))
        {
          /*
           * Upping is when we move from direct to singly indirect.
           */
          bool upping;
          upping = map->size.count == RTEMS_RFS_INODE_BLOCKS;
          rc = rtems_rfs_block_map_indirect_alloc (fs, map,
                                                   &map->singly_buffer,
                                                   &map->blocks[singly],
                                                   upping);
        }
        else
        {
          rc = rtems_rfs_buffer_handle_request (fs,  &map->singly_buffer,
                                                map->blocks[singly], true);
        }

        if (rc > 0)
        {
          rtems_rfs_group_bitmap_free (fs, false, block);
          return rc;
        }
      }
      else
      {
        /*
         * Doubly indirect tables are being used.
         */
        rtems_rfs_block_no doubly;
        rtems_rfs_block_no singly_block;

        doubly  = singly / fs->blocks_per_block;
        singly %= fs->blocks_per_block;

        /*
         * Allocate a new block for a singly indirect table if direct is 0 as
         * it is the first entry of a new block. We may also need to allocate a
         * doubly indirect block as well. Both always occur when direct is 0
         * and the doubly indirect block when singly is 0.
         */
        if (direct == 0)
        {
          rc = rtems_rfs_block_map_indirect_alloc (fs, map,
                                                   &map->singly_buffer,
                                                   &singly_block,
                                                   false);
          if (rc > 0)
          {
            rtems_rfs_group_bitmap_free (fs, false, block);
            return rc;
          }

          /*
           * Allocate a new block for a doubly indirect table if singly is 0 as
           * it is the first entry of a new singly indirect block.
           */
          if ((singly == 0) ||
              ((doubly == 0) && (singly == RTEMS_RFS_INODE_BLOCKS)))
          {
            bool upping;
            upping = map->size.count == fs->block_map_singly_blocks;
            rc = rtems_rfs_block_map_indirect_alloc (fs, map,
                                                     &map->doubly_buffer,
                                                     &map->blocks[doubly],
                                                     upping);
            if (rc > 0)
            {
              rtems_rfs_group_bitmap_free (fs, false, singly_block);
              rtems_rfs_group_bitmap_free (fs, false, block);
              return rc;
            }
          }
          else
          {
            rc = rtems_rfs_buffer_handle_request (fs, &map->doubly_buffer,
                                                  map->blocks[doubly], true);
            if (rc > 0)
            {
              rtems_rfs_group_bitmap_free (fs, false, singly_block);
              rtems_rfs_group_bitmap_free (fs, false, block);
              return rc;
            }
          }

          rtems_rfs_block_set_number (&map->doubly_buffer,
                                      singly,
                                      singly_block);
        }
        else
        {
          rc = rtems_rfs_buffer_handle_request (fs,
                                                &map->doubly_buffer,
                                                map->blocks[doubly],
                                                true);
          if (rc > 0)
          {
            rtems_rfs_group_bitmap_free (fs, false, block);
            return rc;
          }

          singly_block = rtems_rfs_block_get_number (&map->doubly_buffer,
                                                     singly);

          rc = rtems_rfs_buffer_handle_request (fs, &map->singly_buffer,
                                                singly_block, true);
          if (rc > 0)
          {
            rtems_rfs_group_bitmap_free (fs, false, block);
            return rc;
          }
        }
      }

      rtems_rfs_block_set_number (&map->singly_buffer, direct, block);
    }

    map->size.count++;
    map->size.offset = 0;

    if (b == 0)
      *new_block = block;
    map->last_data_block = block;
    map->dirty = true;
  }

  return 0;
}

/**
 * Shrink an indirect block.
 *
 * @param fs The file system data.
 * @param map The map the allocation is for.
 * @param buffer The buffer the indirect block is accessed by.
 * @param indirect The index index in the inode's block table.
 * @param index The index in the indirect table of the block.
 * @return int The error number (errno). No error if 0.
 */
static int
rtems_rfs_block_map_indirect_shrink (rtems_rfs_file_system*   fs,
                                     rtems_rfs_block_map*     map,
                                     rtems_rfs_buffer_handle* buffer,
                                     rtems_rfs_block_no       indirect,
                                     rtems_rfs_block_no       index)
{
  int rc = 0;

  /*
   * If this is the first block in the indirect table (index == 0), ie the last
   * block to be freed and the indirect block is now also free, or we have only
   * one indirect table and we can fit the remaining blocks into the inode,
   * then either move to the next indirect block or move the remaining blocks
   * into the inode and free the indirect table's block.
   */
  if ((index == 0) ||
      ((indirect == 0) && (index == RTEMS_RFS_INODE_BLOCKS)))
  {
    rtems_rfs_block_no block_to_free = map->blocks[indirect];

    if ((indirect == 0) && (index == RTEMS_RFS_INODE_BLOCKS))
    {
      /*
       * Move to direct inode access.
       */
      int b;
      for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
        map->blocks[b] = rtems_rfs_block_get_number (buffer, b);
    }
    else
    {
      /*
       * One less singly indirect block in the inode.
       */
      map->blocks[indirect] = 0;
    }

    rc = rtems_rfs_group_bitmap_free (fs, false, block_to_free);
    if (rc > 0)
      return rc;

    map->last_map_block = block_to_free;
  }

  return rc;
}

int
rtems_rfs_block_map_shrink (rtems_rfs_file_system* fs,
                            rtems_rfs_block_map*   map,
                            size_t                 blocks)
{
  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BLOCK_MAP_SHRINK))
    printf ("rtems-rfs: block-map-shrink: entry: blocks=%zd count=%" PRIu32 "\n",
            blocks, map->size.count);

  if (map->size.count == 0)
    return 0;

  if (blocks > map->size.count)
    blocks = map->size.count;

  while (blocks)
  {
    rtems_rfs_block_no block;
    rtems_rfs_block_no block_to_free;
    int                rc;

    block = map->size.count - 1;

    if (block < RTEMS_RFS_INODE_BLOCKS)
    {
      /*
       * We have less than RTEMS_RFS_INODE_BLOCKS so they are held in the
       * inode.
       */
      block_to_free = map->blocks[block];
      map->blocks[block] = 0;
    }
    else
    {
      /*
       * Single indirect access is occuring. It could still be doubly indirect.
       *
       * The 'direct' variable is the offset in to the indirect table of
       * blocks, and 'singly' is the inode block index of the singly indirect
       * table of block numbers.
       */
      rtems_rfs_block_no direct;
      rtems_rfs_block_no singly;

      direct = block % fs->blocks_per_block;
      singly = block / fs->blocks_per_block;

      if (block < fs->block_map_singly_blocks)
      {
        /*
         * Request the indirect block and then obtain the block number from the
         * indirect block.
         */
        rc = rtems_rfs_buffer_handle_request (fs, &map->singly_buffer,
                                              map->blocks[singly], true);
        if (rc > 0)
          return rc;

        block_to_free = rtems_rfs_block_get_number (&map->singly_buffer,
                                                    direct);

        rc = rtems_rfs_block_map_indirect_shrink (fs, map, &map->singly_buffer,
                                                  singly, direct);
        if (rc)
          return rc;
      }
      else if (block < fs->block_map_doubly_blocks)
      {
        /*
         * Doubly indirect tables are being used. The 'doubly' variable is the
         * index in to the inode's block table and points to a singly indirect
         * table of block numbers. The 'doubly_singly' variable is the index
         * into the doubly indirect table pointing to the singly indirect table
         * of block numbers that form the map. This is used later to determine
         * if the current doubly indirect table needs to be freed. The 'direct'
         * value is still valid for doubly indirect tables.
         */
        rtems_rfs_block_no doubly;
        rtems_rfs_block_no doubly_singly;

        doubly        = singly / fs->blocks_per_block;
        doubly_singly = singly % fs->blocks_per_block;

        rc = rtems_rfs_buffer_handle_request (fs, &map->doubly_buffer,
                                              map->blocks[doubly], true);
        if (rc > 0)
          return rc;

        singly = rtems_rfs_block_get_number (&map->doubly_buffer,
                                             doubly_singly);

        /*
         * Read the singly indirect table and get the block number.
         */
        rc = rtems_rfs_buffer_handle_request (fs, &map->singly_buffer,
                                              singly, true);
        if (rc > 0)
          return rc;

        block_to_free = rtems_rfs_block_get_number (&map->singly_buffer,
                                                    direct);

        if (direct == 0)
        {
          rc = rtems_rfs_group_bitmap_free (fs, false, singly);
          if (rc > 0)
            return rc;

          map->last_map_block = singly;

          rc = rtems_rfs_block_map_indirect_shrink (fs, map, &map->doubly_buffer,
                                                    doubly, doubly_singly);
          if (rc)
            return rc;
        }
      }
      else
      {
        rc = EIO;
        break;
      }
    }
    rc = rtems_rfs_group_bitmap_free (fs, false, block_to_free);
    if (rc > 0)
      return rc;
    map->size.count--;
    map->size.offset = 0;
    map->last_data_block = block_to_free;
    map->dirty = true;
    blocks--;
  }

  if (map->size.count == 0)
  {
    map->last_map_block = 0;
    map->last_data_block = 0;
  }

  /*
   * Keep the position inside the map.
   */
  if (rtems_rfs_block_pos_past_end (&map->bpos, &map->size))
    rtems_rfs_block_size_get_bpos (&map->size, &map->bpos);

  return 0;
}

int
rtems_rfs_block_map_free_all (rtems_rfs_file_system* fs,
                              rtems_rfs_block_map*   map)
{
  return rtems_rfs_block_map_shrink (fs, map, map->size.count);
}
