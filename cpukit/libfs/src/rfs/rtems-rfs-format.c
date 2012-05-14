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
 * RTEMS File Systems Format
 *
 * Format the file system ready for use.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>
#include <rtems/rtems-rfs-format.h>
#include <rtems/rfs/rtems-rfs-dir.h>

/**
 * Return the number of gigabytes.
 */
#define GIGS(_g) (((uint64_t)(_g)) * 1024 * 1024)

/**
 * Return the number of bits that fit in the block size.
 */
static int
rtems_rfs_bits_per_block (rtems_rfs_file_system* fs)
{
  return rtems_rfs_bitmap_numof_bits (rtems_rfs_fs_block_size (fs));
}

/**
 * Return a rounded up integer quotient given a dividend and divisor. That is:
 * "quotient = dividend / divisor"
 */
int
rtems_rfs_rup_quotient (uint32_t dividend, uint32_t divisor)
{
  if (dividend == 0)
    return 1;
  return ((dividend - 1) / divisor) + 1;
}

/**
 * Return the number of inodes as a percentage of the total number that can fit
 * in a blocl.
 */
static int
rtems_rfs_inodes_from_percent (rtems_rfs_file_system* fs,
                               int                    percentage)
{
  int blocks;
  blocks = ((rtems_rfs_fs_blocks (fs) -
             RTEMS_RFS_SUPERBLOCK_SIZE) * percentage) / 100;
  blocks = rtems_rfs_rup_quotient (blocks, fs->group_count);
  return blocks * (rtems_rfs_fs_block_size (fs) / RTEMS_RFS_INODE_SIZE);
}

/**
 * Return the inode overhead given a number of inodes.
 */
static int
rtems_rfs_inode_overhead (rtems_rfs_file_system* fs)
{
  int blocks;
  int bits_per_block;
  blocks = rtems_rfs_rup_quotient(fs->group_inodes * RTEMS_RFS_INODE_SIZE,
                                  rtems_rfs_fs_block_size (fs));
  bits_per_block = rtems_rfs_bits_per_block (fs);
  /*
   * There could be more bits than blocks, eg 512K disk with 512 blocks.
   */
  if (bits_per_block > (rtems_rfs_fs_blocks (fs) - RTEMS_RFS_SUPERBLOCK_SIZE))
    bits_per_block = rtems_rfs_fs_blocks (fs) - RTEMS_RFS_SUPERBLOCK_SIZE;
  return ((blocks + 1) * 100 * 10) / bits_per_block;
}

static bool
rtems_rfs_check_config (rtems_rfs_file_system*         fs,
                        const rtems_rfs_format_config* config)
{
  fs->block_size = config->block_size;
  if (!fs->block_size)
  {
    uint64_t total_size = rtems_rfs_fs_media_size (fs);

    if (total_size >= GIGS (1))
    {
      uint32_t gigs = (total_size + GIGS (1)) / GIGS (1);
      int      b;
      for (b = 31; b > 0; b--)
        if ((gigs & (1 << b)) != 0)
          break;
      fs->block_size = 1 << b;
    }

    if (fs->block_size < 512)
      fs->block_size = 512;

    if (fs->block_size > (4 * 1024))
      fs->block_size = (4 * 1024);
  }

  if ((fs->block_size % rtems_rfs_fs_media_block_size (fs)) != 0)
  {
    printf ("block size (%zd) is not a multiple of media block size (%" PRId32 ")\n",
            fs->block_size, rtems_rfs_fs_media_block_size (fs));
    return false;
  }

  fs->group_blocks = config->group_blocks;
  if (!fs->group_blocks)
  {
    /*
     * The number of blocks per group is defined by the number of bits in a
     * block.
     */
    fs->group_blocks = rtems_rfs_bitmap_numof_bits (fs->block_size);
  }

  if (fs->group_blocks > rtems_rfs_bitmap_numof_bits (fs->block_size))
  {
    printf ("group block count is higher than bits in block\n");
    return false;
  }

  fs->blocks = rtems_rfs_fs_media_size (fs) / fs->block_size;

  /*
   * The bits per block sets the upper limit for the number of blocks in a
   * group. The disk will be divided into groups which are the number of bits
   * per block.
   */
  fs->group_count = rtems_rfs_rup_quotient (rtems_rfs_fs_blocks (fs),
                                            rtems_rfs_bits_per_block (fs));

  fs->group_inodes = config->group_inodes;
  if (!fs->group_inodes)
  {
    int inode_overhead = RTEMS_RFS_INODE_OVERHEAD_PERCENTAGE;

    /*
     * The number of inodes per group is set as a percentage.
     */
    if (config->inode_overhead)
      inode_overhead = config->inode_overhead;

    fs->group_inodes = rtems_rfs_inodes_from_percent (fs, inode_overhead);
  }

  /*
   * Round up to fill a block because the minimum allocation unit is a block.
   */
  fs->inodes_per_block = rtems_rfs_fs_block_size (fs) / RTEMS_RFS_INODE_SIZE;
  fs->group_inodes =
    rtems_rfs_rup_quotient (fs->group_inodes,
                            fs->inodes_per_block) * fs->inodes_per_block;

  if (fs->group_inodes > rtems_rfs_bitmap_numof_bits (fs->block_size))
    fs->group_inodes = rtems_rfs_bitmap_numof_bits (fs->block_size);

  fs->max_name_length = config->max_name_length;
  if (!fs->max_name_length)
  {
    fs->max_name_length = 512;
  }

  return true;
}

static bool
rtems_rfs_write_group (rtems_rfs_file_system* fs,
                       int                    group,
                       bool                   initialise_inodes,
                       bool                   verbose)
{
  rtems_rfs_buffer_handle  handle;
  rtems_rfs_bitmap_control bitmap;
  rtems_rfs_buffer_block   group_base;
  size_t                   group_size;
  int                      blocks;
  int                      b;
  int                      rc;

  group_base = rtems_rfs_fs_block (fs, group, 0);

  if (group_base > rtems_rfs_fs_blocks (fs))
  {
    printf ("rtems-rfs: write-group: group %d base beyond disk limit\n",
            group);
    return false;
  }

  group_size = fs->group_blocks;

  /*
   * Be nice to strange sizes of disks. These are embedded systems after all
   * and nice numbers do not always work out. Let the last block pick up the
   * remainder of the blocks.
   */
  if ((group_base + group_size) > rtems_rfs_fs_blocks (fs))
    group_size = rtems_rfs_fs_blocks (fs) - group_base;

  if (verbose)
    printf ("\rrtems-rfs: format: group %3d: base = %" PRId32 ", size = %zd",
            group, group_base, group_size);

  /*
   * Open a handle and request an empty buffer.
   */
  rc = rtems_rfs_buffer_handle_open (fs, &handle);
  if (rc > 0)
  {
    printf ("\nrtems-rfs: write-group: handle open failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  if (verbose)
    printf (", blocks");

  /*
   * Open the block bitmap using the new buffer.
   */
  rc = rtems_rfs_bitmap_open (&bitmap, fs, &handle, group_size,
                              group_base + RTEMS_RFS_GROUP_BLOCK_BITMAP_BLOCK);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: open block bitmap failed: %d: %s\n",
            group, rc, strerror (rc));
    return false;
  }

  /*
   * Force the whole buffer to a known state. The bit map may not occupy the
   * whole block.
   */
  memset (rtems_rfs_buffer_data (&handle), 0xff, rtems_rfs_fs_block_size (fs));

  /*
   * Clear the bitmap.
   */
  rc = rtems_rfs_bitmap_map_clear_all (&bitmap);
  if (rc > 0)
  {
    rtems_rfs_bitmap_close (&bitmap);
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: block bitmap clear all failed: %d: %s\n",
            group, rc, strerror (rc));
    return false;
  }

  /*
   * Forced allocation of the block bitmap.
   */
  rtems_rfs_bitmap_map_set (&bitmap, RTEMS_RFS_GROUP_BLOCK_BITMAP_BLOCK);

  /*
   * Forced allocation of the inode bitmap.
   */
  rtems_rfs_bitmap_map_set (&bitmap, RTEMS_RFS_GROUP_INODE_BITMAP_BLOCK);

  /*
   * Determine the number of inodes blocks in the group.
   */
  blocks = rtems_rfs_rup_quotient (fs->group_inodes, fs->inodes_per_block);

  /*
   * Forced allocation of the inode blocks which follow the block bitmap.
   */
  for (b = 0; b < blocks; b++)
    rtems_rfs_bitmap_map_set (&bitmap, b + RTEMS_RFS_GROUP_INODE_BLOCK);

  /*
   * Close the block bitmap.
   */
  rc = rtems_rfs_bitmap_close (&bitmap);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: close block bitmap failed: %d: %s\n",
            group, rc, strerror (rc));
    return false;
  }

  rtems_rfs_buffer_mark_dirty (&handle);

  if (verbose)
    printf (", inodes");

  /*
   * Open the inode bitmap using the old buffer. Should release any changes.
   */
  rc = rtems_rfs_bitmap_open (&bitmap, fs, &handle, group_size,
                              group_base + RTEMS_RFS_GROUP_INODE_BITMAP_BLOCK);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: open inode bitmap failed: %d: %s\n",
            group, rc, strerror (rc));
    return false;
  }

  /*
   * Force the whole buffer to a known state. The bit map may not occupy the
   * whole block.
   */
  memset (rtems_rfs_buffer_data (&handle), 0x00, rtems_rfs_fs_block_size (fs));

  /*
   * Clear the inode bitmap.
   */
  rc = rtems_rfs_bitmap_map_clear_all (&bitmap);
  if (rc > 0)
  {
    rtems_rfs_bitmap_close (&bitmap);
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: inode bitmap" \
            " clear all failed: %d: %s\n", group, rc, strerror (rc));
    return false;
  }

  /*
   * Close the inode bitmap.
   */
  rc = rtems_rfs_bitmap_close (&bitmap);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("\nrtems-rfs: write-group: group %3d: close inode" \
            " bitmap failed: %d: %s\n", group, rc, strerror (rc));
    return false;
  }

  rtems_rfs_buffer_mark_dirty (&handle);

  /*
   * Initialise the inode tables if required to do so.
   */
  if (initialise_inodes)
  {
    for (b = 0; b < blocks; b++)
    {
      rc = rtems_rfs_buffer_handle_request (fs, &handle,
                                            group_base + b + RTEMS_RFS_GROUP_INODE_BLOCK,
                                            false);
      if (rc > 0)
      {
        rtems_rfs_buffer_handle_close (fs, &handle);
        printf ("\nrtems-rfs: write-group: group %3d: block %" PRId32 " request failed: %d: %s\n",
                group, group_base + b + RTEMS_RFS_GROUP_INODE_BLOCK,
                rc, strerror (rc));
        return false;
      }

      /*
       * Force the whole buffer to a known state. The bit map may not occupy the
       * whole block.
       */
      memset (rtems_rfs_buffer_data (&handle), 0xff, rtems_rfs_fs_block_size (fs));

      rtems_rfs_buffer_mark_dirty (&handle);
    }
  }

  rc = rtems_rfs_buffer_handle_close (fs, &handle);
  if (rc > 0)
  {
    printf ("\nrtems-rfs: write-group: buffer handle close failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  return true;
}

static bool
rtems_rfs_write_superblock (rtems_rfs_file_system* fs)
{
  rtems_rfs_buffer_handle handle;
  uint8_t*                sb;
  int                     rc;

  rc = rtems_rfs_buffer_handle_open (fs, &handle);
  if (rc > 0)
  {
    printf ("rtems-rfs: write-superblock: handle open failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  rc = rtems_rfs_buffer_handle_request (fs, &handle, 0, false);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("rtems-rfs: write-superblock: request failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  sb = rtems_rfs_buffer_data (&handle);

#define write_sb(_o, _d) rtems_rfs_write_u32(sb + (_o), _d)

  memset (sb, 0xff, rtems_rfs_fs_block_size (fs));

  write_sb (RTEMS_RFS_SB_OFFSET_MAGIC, RTEMS_RFS_SB_MAGIC);
  write_sb (RTEMS_RFS_SB_OFFSET_VERSION, RTEMS_RFS_VERSION);
  write_sb (RTEMS_RFS_SB_OFFSET_BLOCKS, rtems_rfs_fs_blocks (fs));
  write_sb (RTEMS_RFS_SB_OFFSET_BLOCK_SIZE, rtems_rfs_fs_block_size (fs));
  write_sb (RTEMS_RFS_SB_OFFSET_BAD_BLOCKS, fs->bad_blocks);
  write_sb (RTEMS_RFS_SB_OFFSET_MAX_NAME_LENGTH, fs->max_name_length);
  write_sb (RTEMS_RFS_SB_OFFSET_GROUPS, fs->group_count);
  write_sb (RTEMS_RFS_SB_OFFSET_GROUP_BLOCKS, fs->group_blocks);
  write_sb (RTEMS_RFS_SB_OFFSET_GROUP_INODES, fs->group_inodes);
  write_sb (RTEMS_RFS_SB_OFFSET_INODE_SIZE, RTEMS_RFS_INODE_SIZE);

  rtems_rfs_buffer_mark_dirty (&handle);

  rc = rtems_rfs_buffer_handle_release (fs, &handle);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    printf ("rtems-rfs: write-superblock: buffer release failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  rc = rtems_rfs_buffer_handle_close (fs, &handle);
  if (rc > 0)
  {
    printf ("rtems-rfs: write-superblock: buffer handle close failed: %d: %s\n",
            rc, strerror (rc));
    return false;
  }

  return true;
}

static int
rtems_rfs_write_root_dir (const char* name)
{
  rtems_rfs_file_system* fs;
  rtems_rfs_inode_handle inode;
  rtems_rfs_ino          ino;
  int                    rc;

  /*
   * External API so returns -1.
   */
  rc = rtems_rfs_fs_open (name, NULL,
                          RTEMS_RFS_FS_FORCE_OPEN | RTEMS_RFS_FS_NO_LOCAL_CACHE,
                          0, &fs);
  if (rc < 0)
  {
    printf ("rtems-rfs: format: file system open failed: %d: %s\n",
            errno, strerror (errno));
    return -1;
  }

  rc = rtems_rfs_inode_alloc (fs, RTEMS_RFS_ROOT_INO, &ino);
  if (rc > 0)
  {
    printf ("rtems-rfs: format: inode allocation failed: %d: %s\n",
            rc, strerror (rc));
    rtems_rfs_fs_close (fs);
    return rc;
  }

  if (ino != RTEMS_RFS_ROOT_INO)
  {
    printf ("rtems-rfs: format: allocated inode not root ino: %" PRId32 "\n", ino);
    rtems_rfs_fs_close (fs);
    return rc;
  }

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    printf ("rtems-rfs: format: inode open failed: %d: %s\n",
            rc, strerror (rc));
    rtems_rfs_group_bitmap_free (fs, true, ino);
    rtems_rfs_fs_close (fs);
    return rc;
  }

  rc = rtems_rfs_inode_initialise (&inode, 0,
                                   (RTEMS_RFS_S_IFDIR | RTEMS_RFS_S_IRWXU |
                                    RTEMS_RFS_S_IXGRP | RTEMS_RFS_S_IXOTH),
                                   0, 0);
  if (rc > 0)
    printf ("rtems-rfs: format: inode initialise failed: %d: %s\n",
            rc, strerror (rc));

  rc = rtems_rfs_dir_add_entry (fs, &inode, ".", 1, ino);
  if (rc > 0)
    printf ("rtems-rfs: format: directory add failed: %d: %s\n",
            rc, strerror (rc));

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
    printf ("rtems-rfs: format: inode close failed: %d: %s\n",
            rc, strerror (rc));

  rc = rtems_rfs_fs_close (fs);
  if (rc < 0)
    printf ("rtems-rfs: format: file system close failed: %d: %s\n",
            errno, strerror (errno));

  return rc;
}

int
rtems_rfs_format (const char* name, const rtems_rfs_format_config* config)
{
  rtems_rfs_file_system fs;
  int                   group;
  int                   rc;

  if (config->verbose)
    printf ("rtems-rfs: format: %s\n", name);

  memset (&fs, 0, sizeof (rtems_rfs_file_system));

  rtems_chain_initialize_empty (&fs.buffers);
  rtems_chain_initialize_empty (&fs.release);
  rtems_chain_initialize_empty (&fs.release_modified);
  rtems_chain_initialize_empty (&fs.file_shares);

  fs.max_held_buffers = RTEMS_RFS_FS_MAX_HELD_BUFFERS;

  fs.release_count = 0;
  fs.release_modified_count = 0;

  fs.flags = RTEMS_RFS_FS_NO_LOCAL_CACHE;

  /*
   * Open the buffer interface.
   */
  rc = rtems_rfs_buffer_open (name, &fs);
  if (rc > 0)
  {
    printf ("rtems-rfs: format: buffer open failed: %d: %s\n",
            rc, strerror (rc));
    return -1;
  }

  /*
   * Check the media.
   */
  if (rtems_rfs_fs_media_block_size (&fs) == 0)
  {
    printf ("rtems-rfs: media block is invalid: %" PRIu32 "\n",
            rtems_rfs_fs_media_block_size (&fs));
    return -1;
  }

  /*
   * Check the configuration data.
   */
  if (!rtems_rfs_check_config (&fs, config))
    return -1;

  if (config->verbose)
  {
    printf ("rtems-rfs: format: media size = %" PRIu64 "\n",
            rtems_rfs_fs_media_size (&fs));
    printf ("rtems-rfs: format: media blocks = %" PRIu32 "\n",
            rtems_rfs_fs_media_blocks (&fs));
    printf ("rtems-rfs: format: media block size = %" PRIu32 "\n",
            rtems_rfs_fs_media_block_size (&fs));
    printf ("rtems-rfs: format: size = %" PRIu64 "\n",
            rtems_rfs_fs_size (&fs));
    printf ("rtems-rfs: format: blocks = %zu\n",
            rtems_rfs_fs_blocks (&fs));
    printf ("rtems-rfs: format: block size = %zu\n",
            rtems_rfs_fs_block_size (&fs));
    printf ("rtems-rfs: format: bits per block = %u\n",
            rtems_rfs_bits_per_block (&fs));
    printf ("rtems-rfs: format: inode size = %zu\n", RTEMS_RFS_INODE_SIZE);
    printf ("rtems-rfs: format: inodes = %zu (%d.%d%%)\n",
            fs.group_inodes * fs.group_count,
            rtems_rfs_inode_overhead (&fs) / 10,
            rtems_rfs_inode_overhead (&fs) % 10);
    printf ("rtems-rfs: format: groups = %u\n", fs.group_count);
    printf ("rtems-rfs: format: group blocks = %zu\n", fs.group_blocks);
    printf ("rtems-rfs: format: group inodes = %zu\n", fs.group_inodes);
  }

  rc = rtems_rfs_buffer_setblksize (&fs, rtems_rfs_fs_block_size (&fs));
  if (rc > 0)
  {
    printf ("rtems-rfs: format: setting block size failed: %d: %s\n",
            rc, strerror (rc));
    return -1;
  }

  if (!rtems_rfs_write_superblock (&fs))
  {
    printf ("rtems-rfs: format: superblock write failed\n");
    return -1;
  }

  for (group = 0; group < fs.group_count; group++)
    if (!rtems_rfs_write_group (&fs, group,
                                config->initialise_inodes, config->verbose))
      return -1;

  if (config->verbose)
    printf ("\n");

  rc = rtems_rfs_buffer_close (&fs);
  if (rc > 0)
  {
    printf ("rtems-rfs: format: buffer close failed: %d: %s\n",
            rc, strerror (rc));
    return -1;
  }

  rc = rtems_rfs_write_root_dir (name);
  if (rc > 0)
  {
    printf ("rtems-rfs: format: writing root dir failed: %d: %s\n",
            rc, strerror (rc));
    return -1;
  }

  return 0;
}
