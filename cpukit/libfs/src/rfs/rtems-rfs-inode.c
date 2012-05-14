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
 * RTEMS File Systems Inode Routines.
 *
 * These functions manage inodes in the RFS file system. An inode is part of a
 * block that reside after the bitmaps in the group.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>
#include <rtems/rfs/rtems-rfs-dir.h>

int
rtems_rfs_inode_alloc (rtems_rfs_file_system* fs,
                       rtems_rfs_bitmap_bit   goal,
                       rtems_rfs_ino*         ino)
{
  rtems_rfs_bitmap_bit bit;
  int                  rc;
  rc = rtems_rfs_group_bitmap_alloc (fs, goal, true, &bit);
  *ino = bit;
  return rc;
}

int
rtems_rfs_inode_free (rtems_rfs_file_system* fs,
                      rtems_rfs_ino          ino)
{
  rtems_rfs_bitmap_bit bit;
  bit = ino;
  return rtems_rfs_group_bitmap_free (fs, true, bit);
}

int
rtems_rfs_inode_open (rtems_rfs_file_system*  fs,
                      rtems_rfs_ino           ino,
                      rtems_rfs_inode_handle* handle,
                      bool                    load)
{
  int group;
  int gino;
  int index;
  int rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_OPEN))
    printf ("rtems-rfs: inode-open: ino: %" PRIu32 "\n", ino);

  if (ino == RTEMS_RFS_EMPTY_INO)
    return EINVAL;

  if ((ino - RTEMS_RFS_ROOT_INO) > rtems_rfs_fs_inodes (fs))
    return EINVAL;

  handle->ino = ino;
  handle->node = NULL;
  handle->loads = 0;

  gino  = ino - RTEMS_RFS_ROOT_INO;
  group = gino / fs->group_inodes;
  gino  = gino % fs->group_inodes;
  index = (gino / fs->inodes_per_block) + RTEMS_RFS_GROUP_INODE_BLOCK;

  handle->offset = gino % fs->inodes_per_block;
  handle->block  = rtems_rfs_group_block (&fs->groups[group], index);

  rc = rtems_rfs_buffer_handle_open (fs, &handle->buffer);
  if ((rc == 0) && load)
    rc = rtems_rfs_inode_load (fs, handle);
  return rc;
}

int
rtems_rfs_inode_close (rtems_rfs_file_system*  fs,
                       rtems_rfs_inode_handle* handle)
{
  int rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_CLOSE))
    printf ("rtems-rfs: inode-close: ino: %" PRIu32 "\n", handle->ino);

  rc = rtems_rfs_inode_unload (fs, handle, true);

  if ((rc == 0) && (handle->loads > 0))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_CLOSE))
      printf ("rtems-rfs: inode-close: bad loads number: %d\n",
              handle->loads);
    rc = EIO;
  }

  handle->ino = 0;
  return rc;
}

int
rtems_rfs_inode_load (rtems_rfs_file_system*  fs,
                      rtems_rfs_inode_handle* handle)
{
  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_LOAD))
    printf ("rtems-rfs: inode-load: ino=%" PRIu32 " loads=%i loaded=%s\n",
            handle->ino, handle->loads,
            rtems_rfs_inode_is_loaded (handle) ? "yes" : "no");

  /*
   * An inode does not move so once loaded no need to do again.
   */

  if (!rtems_rfs_inode_is_loaded (handle))
  {
    int rc;

    rc = rtems_rfs_buffer_handle_request (fs,&handle->buffer,
                                          handle->block, true);
    if (rc > 0)
      return rc;

    handle->node = rtems_rfs_buffer_data (&handle->buffer);
    handle->node += handle->offset;
  }

  handle->loads++;

  return 0;
}

int
rtems_rfs_inode_unload (rtems_rfs_file_system*  fs,
                        rtems_rfs_inode_handle* handle,
                        bool                    update_ctime)
{
  int rc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_UNLOAD))
    printf ("rtems-rfs: inode-unload: ino=%" PRIu32 " loads=%i loaded=%s\n",
            handle->ino, handle->loads,
            rtems_rfs_inode_is_loaded (handle) ? "yes" : "no");

  if (rtems_rfs_inode_is_loaded (handle))
  {
    if (handle->loads == 0)
      return EIO;

    handle->loads--;

    if (handle->loads == 0)
    {
      /*
       * If the buffer is dirty it will be release. Also set the ctime.
       */
      if (rtems_rfs_buffer_dirty (&handle->buffer) && update_ctime)
        rtems_rfs_inode_set_ctime (handle, time (NULL));
      rc = rtems_rfs_buffer_handle_release (fs, &handle->buffer);
      handle->node = NULL;
    }
  }

  return rc;
}

int
rtems_rfs_inode_create (rtems_rfs_file_system*  fs,
                        rtems_rfs_ino           parent,
                        const char*             name,
                        size_t                  length,
                        uint16_t                mode,
                        uint16_t                links,
                        uid_t                   uid,
                        gid_t                   gid,
                        rtems_rfs_ino*          ino)
{
  rtems_rfs_inode_handle parent_inode;
  rtems_rfs_inode_handle inode;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_CREATE))
  {
    const char* type = "unknown";
    int         c;
    if (RTEMS_RFS_S_ISDIR (mode))
      type = "dir";
    else if (RTEMS_RFS_S_ISCHR (mode))
      type = "char";
    else if (RTEMS_RFS_S_ISBLK (mode))
      type = "block";
    else if (RTEMS_RFS_S_ISREG (mode))
      type = "file";
    else if (RTEMS_RFS_S_ISLNK (mode))
      type = "link";
    printf("rtems-rfs: inode-create: parent:%" PRIu32 " name:", parent);
    for (c = 0; c < length; c++)
      printf ("%c", name[c]);
    printf (" type:%s mode:%04x (%03o)\n", type, mode, mode & ((1 << 10) - 1));
  }

  /*
   * The file type is field within the mode. Check we have a sane mode set.
   */
  switch (mode & RTEMS_RFS_S_IFMT)
  {
    case RTEMS_RFS_S_IFDIR:
    case RTEMS_RFS_S_IFCHR:
    case RTEMS_RFS_S_IFBLK:
    case RTEMS_RFS_S_IFREG:
    case RTEMS_RFS_S_IFLNK:
      break;
    default:
      return EINVAL;
  }

  rc = rtems_rfs_inode_alloc (fs, parent, ino);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_inode_open (fs, *ino, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_inode_free (fs, *ino);
    return rc;
  }

  rc = rtems_rfs_inode_initialise (&inode, links, mode, uid, gid);
  if (rc > 0)
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_inode_free (fs, *ino);
    return rc;
  }

  /*
   * Only handle the specifics of a directory. Let caller handle the others.
   *
   * The inode delete will free the inode.
   */
  if (RTEMS_RFS_S_ISDIR (mode))
  {
    rc = rtems_rfs_dir_add_entry (fs, &inode, ".", 1, *ino);
    if (rc == 0)
      rc = rtems_rfs_dir_add_entry (fs, &inode, "..", 2, parent);
    if (rc > 0)
    {
      rtems_rfs_inode_delete (fs, &inode);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }
  }

  rc = rtems_rfs_inode_open (fs, parent, &parent_inode, true);
  if (rc > 0)
  {
    rtems_rfs_inode_delete (fs, &inode);
    rtems_rfs_inode_close (fs, &inode);
    return rc;
  }

  rc = rtems_rfs_dir_add_entry (fs, &parent_inode, name, length, *ino);
  if (rc > 0)
  {
    rtems_rfs_inode_delete (fs, &inode);
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_inode_close (fs, &parent_inode);
    return rc;
  }

  /*
   * If the node is a directory update the parent link count as the
   * new directory has the '..' link that points to the parent.
   */
  if (RTEMS_RFS_S_ISDIR (mode))
    rtems_rfs_inode_set_links (&parent_inode,
                               rtems_rfs_inode_get_links (&parent_inode) + 1);

  rc = rtems_rfs_inode_close (fs, &parent_inode);
  if (rc > 0)
  {
    rtems_rfs_inode_delete (fs, &inode);
    rtems_rfs_inode_close (fs, &inode);
    return rc;
  }

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_inode_free (fs, *ino);
    return rc;
  }

  return 0;
}

int
rtems_rfs_inode_delete (rtems_rfs_file_system*  fs,
                        rtems_rfs_inode_handle* handle)
{
  int rc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_INODE_DELETE))
    printf("rtems-rfs: inode-delete: ino:%" PRIu32 " loaded:%s\n",
           rtems_rfs_inode_ino (handle),
           rtems_rfs_inode_is_loaded (handle) ? "yes" : "no");

  if (rtems_rfs_inode_is_loaded (handle))
  {
    rtems_rfs_block_map map;

    /*
     * Free the ino number.
     */
    rc = rtems_rfs_inode_free (fs, handle->ino);
    if (rc > 0)
      return rc;

    /*
     * Free the blocks the inode may have attached.
     */
    rc = rtems_rfs_block_map_open (fs, handle, &map);
    if (rc == 0)
    {
      int rrc;
      rrc = rtems_rfs_block_map_free_all (fs, &map);
      rc = rtems_rfs_block_map_close (fs, &map);
      if (rc > 0)
        rrc = rc;
      memset (handle->node, 0xff, RTEMS_RFS_INODE_SIZE);
      rtems_rfs_buffer_mark_dirty (&handle->buffer);
      /*
       * Do the release here to avoid the ctime field being set on a
       * close. Also if there loads is greater then one then other loads
       * active. Forcing the loads count to 0.
       */
      rc = rtems_rfs_buffer_handle_release (fs, &handle->buffer);
      handle->loads = 0;
      handle->node = NULL;
    }
  }
  return rc;
}

int
rtems_rfs_inode_initialise (rtems_rfs_inode_handle* handle,
                            uint16_t                links,
                            uint16_t                mode,
                            uid_t                   uid,
                            gid_t                   gid)
{
  int b;
  rtems_rfs_inode_set_links (handle, links);
  rtems_rfs_inode_set_flags (handle, 0);
  rtems_rfs_inode_set_mode (handle,  mode);
  rtems_rfs_inode_set_uid_gid (handle, uid, gid);
  rtems_rfs_inode_set_block_offset (handle, 0);
  rtems_rfs_inode_set_block_count (handle, 0);
  for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
    rtems_rfs_inode_set_block (handle, b, 0);
  rtems_rfs_inode_set_last_map_block (handle, 0);
  rtems_rfs_inode_set_last_data_block (handle, 0);
  return rtems_rfs_inode_time_stamp_now (handle, true, true);
}

int
rtems_rfs_inode_time_stamp_now (rtems_rfs_inode_handle* handle,
                                bool                    atime,
                                bool                    mtime)
{
  time_t now;
  if (!rtems_rfs_inode_is_loaded (handle))
    return ENXIO;
  now = time (NULL);
  if (atime)
    rtems_rfs_inode_set_atime (handle, now);
  if (mtime)
    rtems_rfs_inode_set_mtime (handle, now);
  return 0;
}

rtems_rfs_pos
rtems_rfs_inode_get_size (rtems_rfs_file_system*  fs,
                          rtems_rfs_inode_handle* handle)
{
  rtems_rfs_block_size size;
  size.count = rtems_rfs_inode_get_block_count (handle);
  size.offset = rtems_rfs_inode_get_block_offset (handle);
  return rtems_rfs_block_get_size (fs, &size);
}
