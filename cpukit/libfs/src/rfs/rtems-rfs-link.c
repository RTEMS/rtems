/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Link Routines
 * 
 * These functions manage links. A link is the addition of a directory entry
 * in a parent directory and incrementing the links count in the inode.
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

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-trace.h>
#include <rtems/rfs/rtems-rfs-dir.h>
#include <rtems/rfs/rtems-rfs-dir-hash.h>
#include <rtems/rfs/rtems-rfs-link.h>

int
rtems_rfs_link (rtems_rfs_file_system* fs,
                const char*            name,
                int                    length,
                rtems_rfs_ino          parent,
                rtems_rfs_ino          target,
                bool                   link_dir)
{
  rtems_rfs_inode_handle parent_inode;
  rtems_rfs_inode_handle target_inode;
  uint16_t               links;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_LINK))
  {
    int c;
    printf ("rtems-rfs: link: parent(%" PRIu32 ") -> ", parent);
    for (c = 0; c < length; c++)
      printf ("%c", name[c]);
    printf ("(%" PRIu32 ")\n", target);
  }

  rc = rtems_rfs_inode_open (fs, target, &target_inode, true);
  if (rc)
    return rc;

  /*
   * If the target inode is a directory and we cannot link directories
   * return a not supported error code.
   */
  if (!link_dir && S_ISDIR (rtems_rfs_inode_get_mode (&target_inode)))
  {
    rtems_rfs_inode_close (fs, &target_inode);
    return ENOTSUP;
  }

  rc = rtems_rfs_inode_open (fs, parent, &parent_inode, true);
  if (rc)
  {
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_dir_add_entry (fs, &parent_inode, name, length, target);
  if (rc > 0)
  {
    rtems_rfs_inode_close (fs, &parent_inode);
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  links = rtems_rfs_inode_get_links (&target_inode) + 1;
  rtems_rfs_inode_set_links (&target_inode, links);

  rc = rtems_rfs_inode_time_stamp_now (&parent_inode, true, true);
  if (rc > 0)
  {
    rtems_rfs_inode_close (fs, &parent_inode);
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_inode_close (fs, &parent_inode);
  if (rc > 0)
  {
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_inode_close (fs, &target_inode);

  return rc;
}

int
rtems_rfs_unlink (rtems_rfs_file_system* fs,
                  rtems_rfs_ino          parent,
                  rtems_rfs_ino          target,
                  uint32_t               doff,
                  rtems_rfs_unlink_dir   dir_mode)
{
  rtems_rfs_inode_handle parent_inode;
  rtems_rfs_inode_handle target_inode;
  uint16_t               links;
  bool                   dir;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
    printf ("rtems-rfs: unlink: parent(%" PRIu32 ") -X-> (%" PRIu32 ")\n", parent, target);

  rc = rtems_rfs_inode_open (fs, target, &target_inode, true);
  if (rc)
    return rc;

  /*
   * If a directory process the unlink mode.
   */

  dir = RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&target_inode));
  if (dir)
  {
    switch (dir_mode)
    {
      case rtems_rfs_unlink_dir_denied:
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
          printf ("rtems-rfs: link is a directory\n");
        rtems_rfs_inode_close (fs, &target_inode);
        return EISDIR;

      case rtems_rfs_unlink_dir_if_empty:
        rc = rtems_rfs_dir_empty (fs, &target_inode);
        if (rc > 0)
        {
          if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
            printf ("rtems-rfs: dir-empty: %d: %s\n", rc, strerror (rc));
          rtems_rfs_inode_close (fs, &target_inode);
          return rc;
        }
        break;

      default:
        break;
    }
  }

  rc = rtems_rfs_inode_open (fs, parent, &parent_inode, true);
  if (rc)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
      printf ("rtems-rfs: link: inode-open failed: %d: %s\n",
              rc, strerror (rc));
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_dir_del_entry (fs, &parent_inode, target, doff);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
      printf ("rtems-rfs: unlink: dir-del failed: %d: %s\n",
              rc, strerror (rc));
    rtems_rfs_inode_close (fs, &parent_inode);
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  links = rtems_rfs_inode_get_links (&target_inode);

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
    printf ("rtems-rfs: unlink: target:%" PRIu32 " links:%u\n", target, links);

  if (links > 1)
  {
    links--;
    rtems_rfs_inode_set_links (&target_inode, links);
  }
  else
  {
    /*
     * Erasing the inode releases all blocks attached to it.
     */
    rc = rtems_rfs_inode_delete (fs, &target_inode);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
        printf ("rtems-rfs: unlink: inode-del failed: %d: %s\n",
                rc, strerror (rc));
      rtems_rfs_inode_close (fs, &parent_inode);
      rtems_rfs_inode_close (fs, &target_inode);
      return rc;
    }

    if (dir)
    {
      links = rtems_rfs_inode_get_links (&parent_inode);
      if (links > 1)
        links--;
      rtems_rfs_inode_set_links (&parent_inode, links);
    }
  }

  rc = rtems_rfs_inode_time_stamp_now (&parent_inode, true, true);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
      printf ("rtems-rfs: link: inode-time-stamp failed: %d: %s\n",
              rc, strerror (rc));
    rtems_rfs_inode_close (fs, &parent_inode);
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_inode_close (fs, &parent_inode);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
      printf ("rtems-rfs: link: parent inode-close failed: %d: %s\n",
              rc, strerror (rc));
    rtems_rfs_inode_close (fs, &target_inode);
    return rc;
  }

  rc = rtems_rfs_inode_close (fs, &target_inode);

  if ((rc > 0) && rtems_rfs_trace (RTEMS_RFS_TRACE_UNLINK))
    printf ("rtems-rfs: link: target inode-close failed: %d: %s\n",
            rc, strerror (rc));

  return rc;
}

int
rtems_rfs_symlink (rtems_rfs_file_system* fs,
                   const char*            name,
                   int                    length,
                   const char*            link,
                   int                    link_length,
                   uid_t                  uid,
                   gid_t                  gid,
                   rtems_rfs_ino          parent)
{
  rtems_rfs_inode_handle inode;
  rtems_rfs_ino          ino;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_SYMLINK))
  {
    int c;
    printf ("rtems-rfs: symlink: parent:%" PRIu32 " name:", parent);
    for (c = 0; c < length; c++)
      printf ("%c", name[c]);
    printf (" link:");
    for (c = 0; c < link_length; c++)
      printf ("%c", link[c]);
  }

  if (link_length >= rtems_rfs_fs_block_size (fs))
    return ENAMETOOLONG;

  rc = rtems_rfs_inode_create (fs, parent, name, strlen (name),
                               RTEMS_RFS_S_SYMLINK,
                               1, uid, gid, &ino);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
    return rc;

  /*
   * If the link length is less than the length of data union in the inode
   * place the link into the data area else allocate a block and write the link
   * to that.
   */
  if (link_length < RTEMS_RFS_INODE_DATA_NAME_SIZE)
  {
    memset (inode.node->data.name, 0, RTEMS_RFS_INODE_DATA_NAME_SIZE);
    memcpy (inode.node->data.name, link, link_length);
    rtems_rfs_inode_set_block_count (&inode, 0);
  }
  else
  {
    rtems_rfs_block_map     map;
    rtems_rfs_block_no      block;
    rtems_rfs_buffer_handle buffer;
    uint8_t*                data;

    rc = rtems_rfs_block_map_open (fs, &inode, &map);
    if (rc > 0)
    {
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_block_map_grow (fs, &map, 1, &block);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_buffer_handle_open (fs, &buffer);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, false);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    data = rtems_rfs_buffer_data (&buffer);

    memset (data, 0xff, rtems_rfs_fs_block_size (fs));
    memcpy (data, link, link_length);

    rc = rtems_rfs_buffer_handle_close (fs, &buffer);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_block_map_close (fs, &map);
    if (rc > 0)
    {
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }
  }

  rtems_rfs_inode_set_block_offset (&inode, link_length);

  rc = rtems_rfs_inode_close (fs, &inode);

  return rc;
}

int
rtems_rfs_symlink_read (rtems_rfs_file_system* fs,
                        rtems_rfs_ino          link,
                        char*                  path,
                        size_t                 size,
                        size_t*                length)
{
  rtems_rfs_inode_handle inode;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_SYMLINK_READ))
    printf ("rtems-rfs: symlink-read: link:%" PRIu32 "\n", link);

  rc = rtems_rfs_inode_open (fs, link, &inode, true);
  if (rc)
    return rc;

  if (!RTEMS_RFS_S_ISLNK (rtems_rfs_inode_get_mode (&inode)))
  {
    rtems_rfs_inode_close (fs, &inode);
    return EINVAL;
  }

  *length = rtems_rfs_inode_get_block_offset (&inode);

  if (size < *length)
  {
    *length = size;
  }

  if (rtems_rfs_inode_get_block_count (&inode) == 0)
  {
    memcpy (path, inode.node->data.name, *length);
  }
  else
  {
    rtems_rfs_block_map     map;
    rtems_rfs_block_no      block;
    rtems_rfs_buffer_handle buffer;
    char*                   data;

    rc = rtems_rfs_block_map_open (fs, &inode, &map);
    if (rc > 0)
    {
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_block_map_seek (fs, &map, 0, &block);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_buffer_handle_open (fs, &buffer);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, false);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    data = rtems_rfs_buffer_data (&buffer);
    memcpy (path, data, *length);

    rc = rtems_rfs_buffer_handle_close (fs, &buffer);
    if (rc > 0)
    {
      rtems_rfs_block_map_close (fs, &map);
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }

    rc = rtems_rfs_block_map_close (fs, &map);
    if (rc > 0)
    {
      rtems_rfs_inode_close (fs, &inode);
      return rc;
    }
  }

  rc = rtems_rfs_inode_close (fs, &inode);

  return rc;
}
