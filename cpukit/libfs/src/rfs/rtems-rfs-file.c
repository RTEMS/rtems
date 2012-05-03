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
 * RTEMS File Systems File Routines.
 *
 * These functions manage files.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-block-pos.h>
#include <rtems/rfs/rtems-rfs-file.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-trace.h>

int
rtems_rfs_file_open (rtems_rfs_file_system*  fs,
                     rtems_rfs_ino           ino,
                     int                     oflag,
                     rtems_rfs_file_handle** file)
{
  rtems_rfs_file_handle* handle;
  rtems_rfs_file_shared* shared;
  int                    rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
    printf ("rtems-rfs: file-open: ino=%" PRId32 "\n", ino);

  *file = NULL;

  /*
   * Allocate a new handle and initialise it. Do this before we deal with the
   * shared node data so we do not have to be concerned with reference
   * counting.
   */
  handle = malloc (sizeof (rtems_rfs_file_handle));
  if (!handle)
    return ENOMEM;

  memset (handle, 0, sizeof (rtems_rfs_file_handle));

  rc = rtems_rfs_buffer_handle_open (fs, &handle->buffer);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
      printf ("rtems-rfs: file-open: buffer handle open failed: %d: %s\n",
              rc, strerror (rc));
    free (handle);
    return rc;
  }

  /*
   * Scan the file system data list of open files for this ino. If found up
   * the reference count and return the pointer to the data.
   */
  shared = rtems_rfs_file_get_shared (fs, ino);
  if (shared)
  {
    shared->references++;
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
      printf ("rtems-rfs: file-open: ino=%" PRId32 " shared\n", ino);
  }
  else
  {
    /*
     * None exists so create. Copy in the shared parts of the inode we hold in
     * memory.
     */
    shared = malloc (sizeof (rtems_rfs_file_shared));
    if (!shared)
    {
      rtems_rfs_buffer_handle_close (fs, &handle->buffer);
      free (handle);
      return ENOMEM;
    }

    memset (shared, 0, sizeof (rtems_rfs_file_shared));

    rc = rtems_rfs_inode_open (fs, ino, &shared->inode, true);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
        printf ("rtems-rfs: file-open: inode open failed: %d: %s\n",
                rc, strerror (rc));
      free (shared);
      rtems_rfs_buffer_handle_close (fs, &handle->buffer);
      free (handle);
      return rc;
    }

    rc = rtems_rfs_block_map_open (fs, &shared->inode, &shared->map);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
        printf ("rtems-rfs: file-open: block map open failed: %d: %s\n",
                rc, strerror (rc));
      rtems_rfs_inode_close (fs, &shared->inode);
      free (shared);
      rtems_rfs_buffer_handle_close (fs, &handle->buffer);
      free (handle);
      return rc;
    }

    shared->references = 1;
    shared->size.count = rtems_rfs_inode_get_block_count (&shared->inode);
    shared->size.offset = rtems_rfs_inode_get_block_offset (&shared->inode);
    shared->atime = rtems_rfs_inode_get_atime (&shared->inode);
    shared->mtime = rtems_rfs_inode_get_mtime (&shared->inode);
    shared->ctime = rtems_rfs_inode_get_ctime (&shared->inode);
    shared->fs = fs;

    rtems_chain_append (&fs->file_shares, &shared->link);

    rtems_rfs_inode_unload (fs, &shared->inode, false);

    if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_OPEN))
      printf ("rtems-rfs: file-open: ino=%" PRId32 " share created\n", ino);
  }

  handle->flags  = oflag;
  handle->shared = shared;

  *file = handle;

  return 0;
}

int
rtems_rfs_file_close (rtems_rfs_file_system* fs,
                      rtems_rfs_file_handle* handle)
{
  int rrc;
  int rc;

  rrc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_CLOSE))
    printf ("rtems-rfs: file-close: entry: ino=%" PRId32 "\n",
            handle->shared->inode.ino);

  if (handle->shared->references > 0)
    handle->shared->references--;

  if (handle->shared->references == 0)
  {
    if (!rtems_rfs_inode_is_loaded (&handle->shared->inode))
      rrc = rtems_rfs_inode_load (fs, &handle->shared->inode);

    if (rrc == 0)
    {
      /*
       * @todo This could be clever and only update if different.
       */
      rtems_rfs_inode_set_atime (&handle->shared->inode,
                                 handle->shared->atime);
      rtems_rfs_inode_set_mtime (&handle->shared->inode,
                                 handle->shared->mtime);
      rtems_rfs_inode_set_ctime (&handle->shared->inode,
                                 handle->shared->ctime);
      if (!rtems_rfs_block_size_equal (&handle->shared->size,
                                       &handle->shared->map.size))
        rtems_rfs_block_map_set_size (&handle->shared->map,
                                      &handle->shared->size);
    }

    rc = rtems_rfs_block_map_close (fs, &handle->shared->map);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_CLOSE))
        printf ("rtems-rfs: file-close: map close error: ino=%" PRId32 ": %d: %s\n",
                handle->shared->inode.ino, rc, strerror (rc));
      if (rrc == 0)
        rrc = rc;
    }

    rc = rtems_rfs_inode_close (fs, &handle->shared->inode);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_CLOSE))
        printf ("rtems-rfs: file-close: inode close error: ino=%" PRId32 ": %d: %s\n",
                handle->shared->inode.ino, rc, strerror (rc));
      if (rrc == 0)
        rrc = rc;
    }

    rtems_chain_extract (&handle->shared->link);
    free (handle->shared);
  }

  rc = rtems_rfs_buffer_handle_close (fs, &handle->buffer);
  if ((rrc == 0) && (rc > 0))
    rrc = rc;

  if (rrc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_CLOSE))
      printf ("rtems-rfs: file-close: result: %d: %s\n", rrc, strerror (rrc));
  }

  free (handle);

  return rrc;
}

int
rtems_rfs_file_io_start (rtems_rfs_file_handle* handle,
                         size_t*                available,
                         bool                   read)
{
  size_t size;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-io: start: %s pos=%" PRIu32 ":%" PRIu32 "\n",
            read ? "read" : "write",  handle->bpos.bno, handle->bpos.boff);

  if (!rtems_rfs_buffer_handle_has_block (&handle->buffer))
  {
    rtems_rfs_buffer_block block;
    bool                   request_read;
    int                    rc;

    request_read = read;

    rc = rtems_rfs_block_map_find (rtems_rfs_file_fs (handle),
                                   rtems_rfs_file_map (handle),
                                   rtems_rfs_file_bpos (handle),
                                   &block);
    if (rc > 0)
    {
      /*
       * Has the read reached the EOF ?
       */
      if (read && (rc == ENXIO))
      {
        *available = 0;
        return 0;
      }

      if (rc != ENXIO)
        return rc;

      if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
        printf ("rtems-rfs: file-io: start: grow\n");

      rc = rtems_rfs_block_map_grow (rtems_rfs_file_fs (handle),
                                     rtems_rfs_file_map (handle),
                                     1, &block);
      if (rc > 0)
        return rc;

      request_read = false;
    }
    else
    {
      /*
       * If this is a write check if the write starts within a block or the
       * amount of data is less than a block size. If it is read the block
       * rather than getting a block to fill.
       */
      if (!read &&
          (rtems_rfs_file_block_offset (handle) ||
           (*available < rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle)))))
        request_read = true;
    }

    if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
      printf ("rtems-rfs: file-io: start: block=%" PRIu32 " request-read=%s\n",
              block, request_read ? "yes" : "no");

    rc = rtems_rfs_buffer_handle_request (rtems_rfs_file_fs (handle),
                                          rtems_rfs_file_buffer (handle),
                                          block, request_read);
    if (rc > 0)
      return rc;
  }

  if (read
      && rtems_rfs_block_map_last (rtems_rfs_file_map (handle))
      && rtems_rfs_block_map_size_offset (rtems_rfs_file_map (handle)))
    size = rtems_rfs_block_map_size_offset (rtems_rfs_file_map (handle));
  else
    size = rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle));

  *available = size - rtems_rfs_file_block_offset (handle);

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-io: start: available=%zu (%zu)\n",
            *available, size);

  return 0;
}

int
rtems_rfs_file_io_end (rtems_rfs_file_handle* handle,
                       size_t                 size,
                       bool                   read)
{
  bool atime;
  bool mtime;
  bool length;
  int  rc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-io:   end: %s size=%zu\n",
            read ? "read" : "write", size);

  if (rtems_rfs_buffer_handle_has_block (&handle->buffer))
  {
    if (!read)
      rtems_rfs_buffer_mark_dirty (rtems_rfs_file_buffer (handle));
    rc = rtems_rfs_buffer_handle_release (rtems_rfs_file_fs (handle),
                                          rtems_rfs_file_buffer (handle));
    if (rc > 0)
    {
      printf (
        "rtems-rfs: file-io:   end: error on release: %s size=%zu: %d: %s\n",
        read ? "read" : "write", size, rc, strerror (rc));

      return rc;
    }
  }

  /*
   * Update the handle's position. Only a block size can be handled at a time
   * so no special maths is needed. If the offset is bigger than the block size
   * increase the block number and adjust the offset.
   *
   * If we are the last block and the position is past the current size update
   * the size with the new length. The map holds the block count.
   */
  handle->bpos.boff += size;

  if (handle->bpos.boff >=
      rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle)))
  {
    handle->bpos.bno++;
    handle->bpos.boff -= rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle));
  }

  length = false;
  mtime = false;

  if (!read &&
      rtems_rfs_block_map_past_end (rtems_rfs_file_map (handle),
                                    rtems_rfs_file_bpos (handle)))
  {
    rtems_rfs_block_map_set_size_offset (rtems_rfs_file_map (handle),
                                         handle->bpos.boff);
    length = true;
    mtime = true;
  }

  atime  = rtems_rfs_file_update_atime (handle);
  mtime  = rtems_rfs_file_update_mtime (handle) && mtime;
  length = rtems_rfs_file_update_length (handle) && length;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-io:   end: pos=%" PRIu32 ":%" PRIu32 " %c %c %c\n",
            handle->bpos.bno, handle->bpos.boff,
            atime ? 'A' : '-', mtime ? 'M' : '-', length ? 'L' : '-');

  if (atime || mtime)
  {
    time_t now = time (NULL);
    if (read && atime)
      handle->shared->atime = now;
    if (!read && mtime)
      handle->shared->mtime = now;
  }
  if (length)
  {
    handle->shared->size.count =
      rtems_rfs_block_map_count (rtems_rfs_file_map (handle));
    handle->shared->size.offset =
      rtems_rfs_block_map_size_offset (rtems_rfs_file_map (handle));
  }

  return rc;
}

int
rtems_rfs_file_io_release (rtems_rfs_file_handle* handle)
{
  int rc = 0;
  if (rtems_rfs_buffer_handle_has_block (&handle->buffer))
    rc = rtems_rfs_buffer_handle_release (rtems_rfs_file_fs (handle),
                                          rtems_rfs_file_buffer (handle));
  return rc;
}

int
rtems_rfs_file_seek (rtems_rfs_file_handle* handle,
                     rtems_rfs_pos          pos,
                     rtems_rfs_pos*         new_pos)
{
  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-seek: new=%" PRIu64 "\n", pos);

  /*
   * This call only sets the position if it is in a valid part of the file. The
   * user can request past the end of the file then write to extend the
   * file. The lseek entry states:
   *
   *    "Although lseek() may position the file offset beyond the end of the
   *     file, this function does not itself extend the size of the file."
   *
   * This means the file needs to set the file size to the pos only when a
   * write occurs.
   */
  if (pos <= rtems_rfs_file_shared_get_size (rtems_rfs_file_fs (handle),
                                            handle->shared))
  {
    rtems_rfs_file_set_bpos (handle, pos);
    
    /*
     * If the file has a block check if it maps to the current position and it
     * does not release it. That will force us to get the block at the new
     * position when the I/O starts.
     */
    if (rtems_rfs_buffer_handle_has_block (&handle->buffer))
    {
      rtems_rfs_buffer_block block;
      int                    rc;
      
      rc = rtems_rfs_block_map_find (rtems_rfs_file_fs (handle),
                                     rtems_rfs_file_map (handle),
                                     rtems_rfs_file_bpos (handle),
                                     &block);
      if (rc > 0)
        return rc;
      if (rtems_rfs_buffer_bnum (&handle->buffer) != block)
      {        
        rc = rtems_rfs_buffer_handle_release (rtems_rfs_file_fs (handle),
                                              rtems_rfs_file_buffer (handle));
        if (rc > 0)
          return rc;
      }
    }
  }
  else
  {
    /*
     * The seek is outside the current file so release any buffer. A write will
     * extend the file.
     */
    int rc = rtems_rfs_file_io_release (handle);
    if (rc > 0)
      return rc;
  }
  
  *new_pos = pos;
  return 0;
}

int
rtems_rfs_file_set_size (rtems_rfs_file_handle* handle,
                         rtems_rfs_pos          new_size)
{
  rtems_rfs_block_map* map  = rtems_rfs_file_map (handle);
  rtems_rfs_pos        size;
  int                  rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_FILE_IO))
    printf ("rtems-rfs: file-set-size: size=%" PRIu64 "\n", new_size);

  size = rtems_rfs_file_size (handle);
  
  /*
   * If the file is same size do nothing else grow or shrink it ?
   *
   * If the file does not change size do not update the times.
   */
  if (size != new_size)
  {
    /*
     * Short cut for the common truncate on open call.
     */
    if (new_size == 0)
    {
      rc = rtems_rfs_block_map_free_all (rtems_rfs_file_fs (handle), map);
      if (rc > 0)
        return rc;
    }
    else
    {
      if (size < new_size)
      {
        /*
         * Grow. Fill with 0's.
         */
        rtems_rfs_pos count;
        uint32_t      length;
        bool          read_block;

        count = new_size - size;
        length = rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle));
        read_block = false;

        while (count)
        {
          rtems_rfs_buffer_block block;
          rtems_rfs_block_pos    bpos;
          uint8_t*               dst;

          /*
           * Get the block position for the current end of the file as seen by
           * the map. If not found and the EOF grow the map then fill the block
           * with 0.
           */
          rtems_rfs_block_size_get_bpos (rtems_rfs_block_map_size (map), &bpos);
          rc = rtems_rfs_block_map_find (rtems_rfs_file_fs (handle),
                                         map, &bpos, &block);
          if (rc > 0)
          {
            /*
             * Have we reached the EOF ?
             */
            if (rc != ENXIO)
              return rc;

            rc = rtems_rfs_block_map_grow (rtems_rfs_file_fs (handle),
                                           map, 1, &block);
            if (rc > 0)
              return rc;
          }

          if (count < (length - bpos.boff))
          {
            length = count + bpos.boff;
            read_block = true;
            rtems_rfs_block_map_set_size_offset (map, length);
          }
          else
          {
            rtems_rfs_block_map_set_size_offset (map, 0);
          }

          /*
           * Only read the block if the length is not the block size.
           */
          rc = rtems_rfs_buffer_handle_request (rtems_rfs_file_fs (handle),
                                                rtems_rfs_file_buffer (handle),
                                                block, read_block);
          if (rc > 0)
            return rc;

          dst = rtems_rfs_buffer_data (&handle->buffer);
          memset (dst + bpos.boff, 0, length - bpos.boff);

          rtems_rfs_buffer_mark_dirty (rtems_rfs_file_buffer (handle));

          rc = rtems_rfs_buffer_handle_release (rtems_rfs_file_fs (handle),
                                                rtems_rfs_file_buffer (handle));
          if (rc > 0)
            return rc;

          count -= length - bpos.boff;
        }
      }
      else
      {
        /*
         * Shrink
         */
        rtems_rfs_block_no blocks;
        uint32_t           offset;

        blocks =
          rtems_rfs_block_map_count (map) -
          (((new_size - 1) /
            rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle))) + 1);

        offset =
          new_size % rtems_rfs_fs_block_size (rtems_rfs_file_fs (handle));

        if (blocks)
        {
          int rc;
          rc = rtems_rfs_block_map_shrink (rtems_rfs_file_fs (handle),
                                           rtems_rfs_file_map (handle),
                                           blocks);
          if (rc > 0)
            return rc;
        }

        rtems_rfs_block_map_set_size_offset (map, offset);

        if (rtems_rfs_block_pos_past_end (rtems_rfs_file_bpos (handle),
                                          rtems_rfs_block_map_size (map)))
          rtems_rfs_block_size_get_bpos (rtems_rfs_block_map_size (map),
                                         rtems_rfs_file_bpos (handle));
      }
    }

    handle->shared->size.count  = rtems_rfs_block_map_count (map);
    handle->shared->size.offset = rtems_rfs_block_map_size_offset (map);

    if (rtems_rfs_file_update_mtime (handle))
      handle->shared->mtime = time (NULL);
  }
  
  return 0;
}

rtems_rfs_file_shared*
rtems_rfs_file_get_shared (rtems_rfs_file_system* fs,
                           rtems_rfs_ino          ino)
{
  rtems_chain_node* node;
  node = rtems_chain_first (&fs->file_shares);
  while (!rtems_chain_is_tail (&fs->file_shares, node))
  {
    rtems_rfs_file_shared* shared;
    shared = (rtems_rfs_file_shared*) node;
    if (shared->inode.ino == ino)
      return shared;
    node = rtems_chain_next (node);
  }
  return NULL;
}
