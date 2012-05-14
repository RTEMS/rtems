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
 * RTEMS File Systems Open
 *
 * Open the file system by reading the superblock and then the group data.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-data.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>
#include <rtems/rfs/rtems-rfs-trace.h>

uint64_t
rtems_rfs_fs_size (rtems_rfs_file_system* fs)
{
  uint64_t blocks = rtems_rfs_fs_blocks (fs);
  uint64_t block_size = rtems_rfs_fs_block_size (fs);
  return blocks * block_size;
}

uint64_t
rtems_rfs_fs_media_size (rtems_rfs_file_system* fs)
{
  uint64_t media_blocks = (uint64_t) rtems_rfs_fs_media_blocks (fs);
  uint64_t media_block_size = (uint64_t) rtems_rfs_fs_media_block_size (fs);
  return media_blocks * media_block_size;
}

static int
rtems_rfs_fs_read_superblock (rtems_rfs_file_system* fs)
{
  rtems_rfs_buffer_handle handle;
  uint8_t*                sb;
  int                     group;
  int                     rc;

  rc = rtems_rfs_buffer_handle_open (fs, &handle);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: handle open failed: %d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  rc = rtems_rfs_buffer_handle_request (fs, &handle, 0, true);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: request failed%d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  sb = rtems_rfs_buffer_data (&handle);

#define read_sb(_o) rtems_rfs_read_u32 (sb + (_o))

  if (read_sb (RTEMS_RFS_SB_OFFSET_MAGIC) != RTEMS_RFS_SB_MAGIC)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: invalid superblock, bad magic\n");
    rtems_rfs_buffer_handle_close (fs, &handle);
    return EIO;
  }

  fs->blocks     = read_sb (RTEMS_RFS_SB_OFFSET_BLOCKS);
  fs->block_size = read_sb (RTEMS_RFS_SB_OFFSET_BLOCK_SIZE);

  if (rtems_rfs_fs_size(fs) > rtems_rfs_fs_media_size (fs))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: invalid superblock block/size count\n");
    rtems_rfs_buffer_handle_close (fs, &handle);
    return EIO;
  }

  if ((read_sb (RTEMS_RFS_SB_OFFSET_VERSION) & RTEMS_RFS_VERSION_MASK) !=
      (RTEMS_RFS_VERSION * RTEMS_RFS_VERSION_MASK))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: incompatible version: %08" PRIx32 " (%08" PRIx32 ")\n",
              read_sb (RTEMS_RFS_SB_OFFSET_VERSION), RTEMS_RFS_VERSION_MASK);
    rtems_rfs_buffer_handle_close (fs, &handle);
    return EIO;
  }

  if (read_sb (RTEMS_RFS_SB_OFFSET_INODE_SIZE) != RTEMS_RFS_INODE_SIZE)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: inode size mismatch: fs:%" PRId32 " target:%" PRId32 "\n",
              read_sb (RTEMS_RFS_SB_OFFSET_VERSION), RTEMS_RFS_VERSION_MASK);
    rtems_rfs_buffer_handle_close (fs, &handle);
    return EIO;
  }

  fs->bad_blocks      = read_sb (RTEMS_RFS_SB_OFFSET_BAD_BLOCKS);
  fs->max_name_length = read_sb (RTEMS_RFS_SB_OFFSET_MAX_NAME_LENGTH);
  fs->group_count     = read_sb (RTEMS_RFS_SB_OFFSET_GROUPS);
  fs->group_blocks    = read_sb (RTEMS_RFS_SB_OFFSET_GROUP_BLOCKS);
  fs->group_inodes    = read_sb (RTEMS_RFS_SB_OFFSET_GROUP_INODES);

  fs->blocks_per_block =
    rtems_rfs_fs_block_size (fs) / sizeof (rtems_rfs_inode_block);

  fs->block_map_singly_blocks =
    fs->blocks_per_block * RTEMS_RFS_INODE_BLOCKS;
  fs->block_map_doubly_blocks =
    fs->blocks_per_block * fs->blocks_per_block * RTEMS_RFS_INODE_BLOCKS;

  fs->inodes = fs->group_count * fs->group_inodes;

  fs->inodes_per_block = fs->block_size / RTEMS_RFS_INODE_SIZE;

  if (fs->group_blocks >
      rtems_rfs_bitmap_numof_bits (rtems_rfs_fs_block_size (fs)))
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: groups blocks larger than block bits\n");
    return EIO;
  }

  rtems_rfs_buffer_handle_close (fs, &handle);

  /*
   * Change the block size to the value in the superblock.
   */
  rc = rtems_rfs_buffer_setblksize (fs, rtems_rfs_fs_block_size (fs));
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: invalid superblock block size%d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  fs->groups = calloc (fs->group_count, sizeof (rtems_rfs_group));

  if (!fs->groups)
  {
    rtems_rfs_buffer_handle_close (fs, &handle);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: read-superblock: no memory for group table\n");
    return ENOMEM;
  }

  /*
   * Perform each phase of group initialisation at the same time. This way we
   * know how far the initialisation has gone if an error occurs and we need to
   * close everything.
   */
  for (group = 0; group < fs->group_count; group++)
  {
    rc = rtems_rfs_group_open (fs,
                               rtems_rfs_fs_block (fs, group, 0),
                               fs->group_blocks,
                               fs->group_inodes,
                               &fs->groups[group]);
    if (rc > 0)
    {
      int g;
      for (g = 0; g < group; g++)
        rtems_rfs_group_close (fs, &fs->groups[g]);
      rtems_rfs_buffer_handle_close (fs, &handle);
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
        printf ("rtems-rfs: read-superblock: no memory for group table%d: %s\n",
                rc, strerror (rc));
      return rc;
    }
  }

  return 0;
}

int
rtems_rfs_fs_open (const char*             name,
                   void*                   user,
                   uint32_t                flags,
                   uint32_t                max_held_buffers,
                   rtems_rfs_file_system** fs)
{
#if UNUSED
  rtems_rfs_group*       group;
  size_t                 group_base;
#endif
  rtems_rfs_inode_handle inode;
  uint16_t               mode;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
    printf ("rtems-rfs: open: %s\n", name);

  *fs = malloc (sizeof (rtems_rfs_file_system));
  if (!*fs)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: open: no memory for file system data\n");
    errno = ENOMEM;
    return -1;
  }

  memset (*fs, 0, sizeof (rtems_rfs_file_system));

  (*fs)->user = user;
  rtems_chain_initialize_empty (&(*fs)->buffers);
  rtems_chain_initialize_empty (&(*fs)->release);
  rtems_chain_initialize_empty (&(*fs)->release_modified);
  rtems_chain_initialize_empty (&(*fs)->file_shares);

  (*fs)->max_held_buffers = max_held_buffers;
  (*fs)->buffers_count = 0;
  (*fs)->release_count = 0;
  (*fs)->release_modified_count = 0;
  (*fs)->flags = flags;

#if UNUSED
  group = &(*fs)->groups[0];
  group_base = 0;
#endif

  /*
   * Open the buffer interface.
   */
  rc = rtems_rfs_buffer_open (name, *fs);
  if (rc > 0)
  {
    free (*fs);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: open: buffer open failed: %d: %s\n",
              rc, strerror (rc));
    errno = rc;
    return -1;
  }

  rc = rtems_rfs_fs_read_superblock (*fs);
  if (rc > 0)
  {
    rtems_rfs_buffer_close (*fs);
    free (*fs);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: open: reading superblock: %d: %s\n",
              rc, strerror (rc));
    errno = rc;
    return -1;
  }

  rc = rtems_rfs_inode_open (*fs, RTEMS_RFS_ROOT_INO, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_buffer_close (*fs);
    free (*fs);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: open: reading root inode: %d: %s\n",
              rc, strerror (rc));
    errno = rc;
    return -1;
  }

  if (((*fs)->flags & RTEMS_RFS_FS_FORCE_OPEN) == 0)
  {
    mode = rtems_rfs_inode_get_mode (&inode);

    if ((mode == 0xffff) || !RTEMS_RFS_S_ISDIR (mode))
    {
      rtems_rfs_inode_close (*fs, &inode);
      rtems_rfs_buffer_close (*fs);
      free (*fs);
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
        printf ("rtems-rfs: open: invalid root inode mode\n");
      errno = EIO;
      return -1;
    }
  }

  rc = rtems_rfs_inode_close (*fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_buffer_close (*fs);
    free (*fs);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_OPEN))
      printf ("rtems-rfs: open: closing root inode: %d: %s\n", rc, strerror (rc));
    errno = rc;
    return -1;
  }

  errno = 0;
  return 0;
}

int
rtems_rfs_fs_close (rtems_rfs_file_system* fs)
{
  int group;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_CLOSE))
    printf ("rtems-rfs: close\n");

  for (group = 0; group < fs->group_count; group++)
    rtems_rfs_group_close (fs, &fs->groups[group]);

  rtems_rfs_buffer_close (fs);

  free (fs);
  return 0;
}
