/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Group Routines
 *
 * These functions open and close a group as well as manage bit allocations
 * within a group.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <string.h>

#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-group.h>

int
rtems_rfs_group_open (rtems_rfs_file_system* fs,
                      rtems_rfs_buffer_block base,
                      size_t                 size,
                      size_t                 inodes,
                      rtems_rfs_group*       group)
{
  int rc;

  if (base >= rtems_rfs_fs_blocks (fs))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
      printf ("rtems-rfs: group-open: base outside file system range: %d: %s\n",
              EIO, strerror (EIO));
    return EIO;
  }

  if ((base + size) >= rtems_rfs_fs_blocks (fs))
    size = rtems_rfs_fs_blocks (fs) - base;

  /*
   * Limit the inodes to the same size as the blocks. This is what the
   * format does and if this is not done the accounting of inodes does
   * not work. If we are so pushed for inodes that this makes a difference
   * the format configuration needs reviewing.
   */
  if (inodes > size)
    inodes = size;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
    printf ("rtems-rfs: group-open: base=%" PRId32 ", blocks=%zd inodes=%zd\n",
            base, size, inodes);

  group->base = base;
  group->size = size;

  rc = rtems_rfs_buffer_handle_open (fs, &group->block_bitmap_buffer);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
      printf ("rtems-rfs: group-open: could not open block bitmap handle: %d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  rc = rtems_rfs_bitmap_open (&group->block_bitmap, fs,
                              &group->block_bitmap_buffer, size,
                              group->base + RTEMS_RFS_GROUP_BLOCK_BITMAP_BLOCK);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &group->block_bitmap_buffer);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
      printf ("rtems-rfs: group-open: could not open block bitmap: %d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  rc = rtems_rfs_buffer_handle_open (fs, &group->inode_bitmap_buffer);
  if (rc > 0)
  {
    rtems_rfs_bitmap_close (&group->block_bitmap);
    rtems_rfs_buffer_handle_close (fs, &group->block_bitmap_buffer);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
      printf ("rtems-rfs: group-open: could not open inode bitmap handle: %d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  rc = rtems_rfs_bitmap_open (&group->inode_bitmap, fs,
                              &group->inode_bitmap_buffer, inodes,
                              group->base + RTEMS_RFS_GROUP_INODE_BITMAP_BLOCK);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &group->inode_bitmap_buffer);
    rtems_rfs_bitmap_close (&group->block_bitmap);
    rtems_rfs_buffer_handle_close (fs, &group->block_bitmap_buffer);
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_OPEN))
      printf ("rtems-rfs: group-open: could not open inode bitmap: %d: %s\n",
              rc, strerror (rc));
    return rc;
  }

  if (rtems_rfs_fs_release_bitmaps (fs))
  {
    rtems_rfs_bitmap_release_buffer (fs, &group->block_bitmap);
    rtems_rfs_bitmap_release_buffer (fs, &group->inode_bitmap);
  }

  return 0;
}

int
rtems_rfs_group_close (rtems_rfs_file_system* fs, rtems_rfs_group* group)
{
  int result = 0;
  int rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_CLOSE))
    printf ("rtems-rfs: group-close: base=%" PRId32 "\n", group->base);

  /*
   * We need to close as much as possible and also return any error if one
   * occurs but this may result in one even more important error being lost but
   * we cannot OR the errors together so this is a reasonable compromise.
   */
  rc = rtems_rfs_bitmap_close (&group->inode_bitmap);
  if (rc > 0)
    result = rc;
  rc = rtems_rfs_buffer_handle_close (fs, &group->inode_bitmap_buffer);
  if (rc > 0)
    result = rc;
  rc = rtems_rfs_bitmap_close (&group->block_bitmap);
  if (rc > 0)
    result = rc;
  rc = rtems_rfs_buffer_handle_close (fs, &group->block_bitmap_buffer);
  if (rc > 0)
    result = rc;

  return result;
}

int
rtems_rfs_group_bitmap_alloc (rtems_rfs_file_system* fs,
                              rtems_rfs_bitmap_bit   goal,
                              bool                   inode,
                              rtems_rfs_bitmap_bit*  result)
{
  int                  group_start;
  size_t               size;
  rtems_rfs_bitmap_bit bit;
  int                  offset;
  bool                 updown;
  int                  direction;

  if (inode)
  {
    size = fs->group_inodes;
    goal -= RTEMS_RFS_ROOT_INO;
  }
  else
  {
    size = fs->group_blocks;
    /*
     * It is possible for 'goal' to be zero. Any newly created inode will have
     * its 'last_data_block' set to zero, which is then used as 'goal' to
     * allocate new blocks. When that happens, we simply set 'goal' to zero and
     * continue the search from there.
     */
    if (goal >= RTEMS_RFS_ROOT_INO)
        goal -= RTEMS_RFS_ROOT_INO;
  }

  group_start = goal / size;
  bit = (rtems_rfs_bitmap_bit) (goal % size);
  offset = 0;
  updown = true;
  direction = 1;

  /*
   * Try the goal group first and if that group fails try the groups either
   * side until the whole file system has be tried.
   */
  while (true)
  {
    rtems_rfs_bitmap_control* bitmap;
    int                       group;
    bool                      allocated = false;
    int                       rc;

    /*
     * We can start at any location and we move out from that point in each
     * direction. The offset grows until we find a free bit or we hit an end.
     */
    group = group_start + (direction * offset);
    if (offset)
      bit = direction > 0 ? 0 : size - 1;

    /*
     * If we are still looking up and down and if the group is out of range we
     * have reached one end. Stopping looking up and down and just move in the
     * one direction one group at a time.
     */
    if ((group < 0) || (group >= fs->group_count))
    {
      if (!updown)
        break;
      direction = direction > 0 ? -1 : 1;
      updown = false;
      continue;
    }

   if (inode)
      bitmap = &fs->groups[group].inode_bitmap;
    else
      bitmap = &fs->groups[group].block_bitmap;

    rc = rtems_rfs_bitmap_map_alloc (bitmap, bit, &allocated, &bit);
    if (rc > 0)
      return rc;

    if (rtems_rfs_fs_release_bitmaps (fs))
      rtems_rfs_bitmap_release_buffer (fs, bitmap);

    if (allocated)
    {
      if (inode)
        *result = rtems_rfs_group_inode (fs, group, bit);
      else
        *result = rtems_rfs_group_block (&fs->groups[group], bit);
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_BITMAPS))
        printf ("rtems-rfs: group-bitmap-alloc: %s allocated: %" PRId32 "\n",
                inode ? "inode" : "block", *result);
      return 0;
    }

    /*
     * If we are still looking back and forth around the
     * group_start, then alternate the direction and
     * increment the offset on every other iteration.
     * Otherwise we are marching through the groups, so just
     * increment the offset.
     */
    if (updown)
    {
      direction = direction > 0 ? -1 : 1;
      if ( direction == -1 )
        offset++;
    }
    else
    {
       offset++;
    }

  }

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_BITMAPS))
    printf ("rtems-rfs: group-bitmap-alloc: no blocks available\n");

  return ENOSPC;
}

int
rtems_rfs_group_bitmap_free (rtems_rfs_file_system* fs,
                             bool                   inode,
                             rtems_rfs_bitmap_bit   no)
{
  rtems_rfs_bitmap_control* bitmap;
  unsigned int              group;
  rtems_rfs_bitmap_bit      bit;
  size_t                    size;
  int                       rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_BITMAPS))
    printf ("rtems-rfs: group-bitmap-free: %s free: %" PRId32 "\n",
            inode ? "inode" : "block", no);

  if (inode)
  {
    no -= RTEMS_RFS_ROOT_INO;
    size = fs->group_inodes;
  }
  else
  {
    no -= RTEMS_RFS_SUPERBLOCK_SIZE;
    size = fs->group_blocks;
  }

  group = no / size;
  bit = (rtems_rfs_bitmap_bit) (no % size);

  if (inode)
    bitmap = &fs->groups[group].inode_bitmap;
  else
    bitmap = &fs->groups[group].block_bitmap;

  rc = rtems_rfs_bitmap_map_clear (bitmap, bit);

  rtems_rfs_bitmap_release_buffer (fs, bitmap);

  return rc;
}

int
rtems_rfs_group_bitmap_test (rtems_rfs_file_system* fs,
                             bool                   inode,
                             rtems_rfs_bitmap_bit   no,
                             bool*                  state)
{
  rtems_rfs_bitmap_control* bitmap;
  unsigned int              group;
  rtems_rfs_bitmap_bit      bit;
  size_t                    size;
  int                       rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_GROUP_BITMAPS))
    printf ("rtems-rfs: group-bitmap-test: %s test: %" PRId32 "\n",
            inode ? "inode" : "block", no);

  if (inode)
  {
    if ((no < RTEMS_RFS_ROOT_INO) || (no > rtems_rfs_fs_inodes (fs)))
        return EINVAL;
    no -= RTEMS_RFS_ROOT_INO;
    size = fs->group_inodes;
  }
  else
  {
    if ((no < RTEMS_RFS_ROOT_INO) || (no >= rtems_rfs_fs_blocks (fs)))
        return EINVAL;
    no -= RTEMS_RFS_ROOT_INO;
    size = fs->group_blocks;
  }

  group = no / size;
  bit = (rtems_rfs_bitmap_bit) (no % size);

  if (inode)
    bitmap = &fs->groups[group].inode_bitmap;
  else
    bitmap = &fs->groups[group].block_bitmap;

  rc = rtems_rfs_bitmap_map_test (bitmap, bit, state);

  rtems_rfs_bitmap_release_buffer (fs, bitmap);

  return rc;
}

int
rtems_rfs_group_usage (rtems_rfs_file_system* fs,
                       size_t*                blocks,
                       size_t*                inodes)
{
  int g;

  *blocks = 0;
  *inodes = 0;

  for (g = 0; g < fs->group_count; g++)
  {
    rtems_rfs_group* group = &fs->groups[g];
    *blocks +=
      rtems_rfs_bitmap_map_size(&group->block_bitmap) -
      rtems_rfs_bitmap_map_free (&group->block_bitmap);
    *inodes +=
      rtems_rfs_bitmap_map_size (&group->inode_bitmap) -
      rtems_rfs_bitmap_map_free (&group->inode_bitmap);
  }

  if (*blocks > rtems_rfs_fs_blocks (fs))
    *blocks = rtems_rfs_fs_blocks (fs);
  if (*inodes > rtems_rfs_fs_inodes (fs))
    *inodes = rtems_rfs_fs_inodes (fs);

  return 0;
}

