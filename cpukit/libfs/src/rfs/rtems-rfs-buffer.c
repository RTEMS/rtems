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
 * RTEMS File Systems Buffer Routines.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <inttypes.h>
#include <errno.h>
#include <fcntl.h>

#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system.h>

/**
 * Scan the chain for a buffer that matches the block number.
 *
 * @param chain The chain to scan.
 * @param count The number of items on the chain.
 * @param block The block number to find.
 * @return  rtems_rfs_buffer* The buffer if found else NULL.
 */
static rtems_rfs_buffer*
rtems_rfs_scan_chain (rtems_chain_control*   chain,
                      uint32_t*              count,
                      rtems_rfs_buffer_block block)
{
  rtems_rfs_buffer* buffer;
  rtems_chain_node* node;

  node = rtems_chain_last (chain);

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CHAINS))
    printf ("rtems-rfs: buffer-scan: count=%" PRIu32 ", block=%" PRIu32 ": ", *count, block);

  while (!rtems_chain_is_head (chain, node))
  {
    buffer = (rtems_rfs_buffer*) node;

    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CHAINS))
      printf ("%" PRIuPTR " ", ((intptr_t) buffer->user));

    if (((rtems_rfs_buffer_block) ((intptr_t)(buffer->user))) == block)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CHAINS))
        printf (": found block=%" PRIuPTR "\n",
                ((intptr_t)(buffer->user)));

      (*count)--;
      rtems_chain_extract (node);
      rtems_chain_set_off_chain (node);
      return buffer;
    }
    node = rtems_chain_previous (node);
  }

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CHAINS))
    printf (": not found\n");

  return NULL;
}

int
rtems_rfs_buffer_handle_request (rtems_rfs_file_system*   fs,
                                 rtems_rfs_buffer_handle* handle,
                                 rtems_rfs_buffer_block   block,
                                 bool                     read)
{
  int rc;

  /*
   * If the handle has a buffer release it. This allows a handle to be reused
   * without needing to close then open it again.
   */
  if (rtems_rfs_buffer_handle_has_block (handle))
  {
    /*
     * Treat block 0 as special to handle the loading of the super block.
     */
    if (block && (rtems_rfs_buffer_bnum (handle) == block))
      return 0;

    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST))
      printf ("rtems-rfs: buffer-request: handle has buffer: %" PRIu32 "\n",
              rtems_rfs_buffer_bnum (handle));

    rc = rtems_rfs_buffer_handle_release (fs, handle);
    if (rc > 0)
      return rc;
    handle->dirty = false;
    handle->bnum = 0;
  }

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST))
    printf ("rtems-rfs: buffer-request: block=%" PRIu32 "\n", block);

  /*
   * First check to see if the buffer has already been requested and is
   * currently attached to a handle. If it is share the access. A buffer could
   * be shared where different parts of the block have separate functions. An
   * example is an inode block and the file system needs to handle 2 inodes in
   * the same block at the same time.
   */
  if (fs->buffers_count)
  {
    /*
     * Check the active buffer list for shared buffers.
     */
    handle->buffer = rtems_rfs_scan_chain (&fs->buffers,
                                           &fs->buffers_count,
                                           block);
    if (rtems_rfs_buffer_handle_has_block (handle) &&
        rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST))
      printf ("rtems-rfs: buffer-request: buffer shared: refs: %d\n",
              rtems_rfs_buffer_refs (handle) + 1);
  }

  /*
   * If the buffer has not been found check the local cache of released
   * buffers. There are release and released modified lists to preserve the
   * state.
   */
  if (!rtems_rfs_fs_no_local_cache (fs) &&
      !rtems_rfs_buffer_handle_has_block (handle))
  {
    /*
     * Check the local cache of released buffers.
     */
    if (fs->release_count)
      handle->buffer = rtems_rfs_scan_chain (&fs->release,
                                             &fs->release_count,
                                             block);

    if (!rtems_rfs_buffer_handle_has_block (handle) &&
        fs->release_modified_count)
    {
      handle->buffer = rtems_rfs_scan_chain (&fs->release_modified,
                                             &fs->release_modified_count,
                                             block);
      /*
       * If we found a buffer retain the dirty buffer state.
       */
      if (rtems_rfs_buffer_handle_has_block (handle))
        rtems_rfs_buffer_mark_dirty (handle);
    }
  }

  /*
   * If not located we request the buffer from the I/O layer.
   */
  if (!rtems_rfs_buffer_handle_has_block (handle))
  {
    rc = rtems_rfs_buffer_io_request (fs, block, read, &handle->buffer);

    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST))
        printf ("rtems-rfs: buffer-request: block=%" PRIu32 ": bdbuf-%s: %d: %s\n",
                block, read ? "read" : "get", rc, strerror (rc));
      return rc;
    }

    rtems_chain_set_off_chain (rtems_rfs_buffer_link(handle));
  }

  /*
   * Increase the reference count of the buffer.
   */
  rtems_rfs_buffer_refs_up (handle);
  rtems_chain_append (&fs->buffers, rtems_rfs_buffer_link (handle));
  fs->buffers_count++;

  handle->buffer->user = (void*) ((intptr_t) block);
  handle->bnum = block;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST))
    printf ("rtems-rfs: buffer-request: block=%" PRIu32 " bdbuf-%s=%" PRIu32 " refs=%d\n",
            block, read ? "read" : "get", handle->buffer->block,
            handle->buffer->references);

  return 0;
}

int
rtems_rfs_buffer_handle_release (rtems_rfs_file_system*   fs,
                                 rtems_rfs_buffer_handle* handle)
{
  int rc = 0;

  if (rtems_rfs_buffer_handle_has_block (handle))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_RELEASE))
      printf ("rtems-rfs: buffer-release: block=%" PRIu32 " %s refs=%d %s\n",
              rtems_rfs_buffer_bnum (handle),
              rtems_rfs_buffer_dirty (handle) ? "(dirty)" : "",
              rtems_rfs_buffer_refs (handle),
              rtems_rfs_buffer_refs (handle) == 0 ? "BAD REF COUNT" : "");

    if (rtems_rfs_buffer_refs (handle) > 0)
      rtems_rfs_buffer_refs_down (handle);

    if (rtems_rfs_buffer_refs (handle) == 0)
    {
      rtems_chain_extract (rtems_rfs_buffer_link (handle));
      fs->buffers_count--;

      if (rtems_rfs_fs_no_local_cache (fs))
      {
        handle->buffer->user = (void*) 0;
        rc = rtems_rfs_buffer_io_release (handle->buffer,
                                          rtems_rfs_buffer_dirty (handle));
      }
      else
      {
        /*
         * If the total number of held buffers is higher than the configured
         * value remove a buffer from the queue with the most buffers and
         * release. The buffers are held on the queues with the newest at the
         * head.
         *
         * This code stops a large series of transactions causing all the
         * buffers in the cache being held in queues of this file system.
         */
        if ((fs->release_count +
             fs->release_modified_count) >= fs->max_held_buffers)
        {
          rtems_rfs_buffer* buffer;
          bool              modified;

          if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_HANDLE_RELEASE))
            printf ("rtems-rfs: buffer-release: local cache overflow:"
                    " %" PRIu32 "\n", fs->release_count + fs->release_modified_count);

          if (fs->release_count > fs->release_modified_count)
          {
            buffer = (rtems_rfs_buffer*) rtems_chain_get (&fs->release);
            fs->release_count--;
            modified = false;
          }
          else
          {
            buffer =
              (rtems_rfs_buffer*) rtems_chain_get (&fs->release_modified);
            fs->release_modified_count--;
            modified = true;
          }
          buffer->user = (void*) 0;
          rc = rtems_rfs_buffer_io_release (buffer, modified);
        }

        if (rtems_rfs_buffer_dirty (handle))
        {
          rtems_chain_append (&fs->release_modified,
                              rtems_rfs_buffer_link (handle));
          fs->release_modified_count++;
        }
        else
        {
          rtems_chain_append (&fs->release, rtems_rfs_buffer_link (handle));
          fs->release_count++;
        }
      }
    }
    handle->buffer = NULL;
  }

  return rc;
}

int
rtems_rfs_buffer_open (const char* name, rtems_rfs_file_system* fs)
{
  struct stat st;
#if RTEMS_RFS_USE_LIBBLOCK
  int rv;
#endif

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SYNC))
    printf ("rtems-rfs: buffer-open: opening: %s\n", name);

  fs->device = open (name, O_RDWR);
  if (fs->device < 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_OPEN))
      printf ("rtems-rfs: buffer-open: cannot open file\n");
    return ENXIO;
  }

  if (fstat (fs->device, &st) < 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_OPEN))
      printf ("rtems-rfs: buffer-open: stat '%s' failed: %s\n",
              name, strerror (errno));
    return ENXIO;
  }

#if RTEMS_RFS_USE_LIBBLOCK
  /*
   * Is the device a block device ?
   */
  if (!S_ISBLK (st.st_mode))
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_OPEN))
      printf ("rtems-rfs: buffer-open: '%s' is not a block device\n", name);
    return ENXIO;
  }

  /*
   * Check that device is registred as a block device and lock it.
   */
  rv = rtems_disk_fd_get_disk_device (fs->device, &fs->disk);
  if (rv != 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_OPEN))
      printf ("rtems-rfs: buffer-open: cannot obtain the disk\n");
    return ENXIO;
  }
#else
  fs->media_size = st.st_size;
  strcat (fs->name, name);
#endif

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SYNC))
    printf ("rtems-rfs: buffer-open: blks=%" PRId32 ", blk-size=%" PRId32 "\n",
            rtems_rfs_fs_media_blocks (fs),
            rtems_rfs_fs_media_block_size (fs));

  return 0;
}

int
rtems_rfs_buffer_close (rtems_rfs_file_system* fs)
{
  int rc = 0;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CLOSE))
    printf ("rtems-rfs: buffer-close: closing\n");

  /*
   * Change the block size to the media device size. It will release and sync
   * all buffers.
   */
  rc = rtems_rfs_buffer_setblksize (fs, rtems_rfs_fs_media_block_size (fs));

  if ((rc > 0) && rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CLOSE))
    printf ("rtems-rfs: buffer-close: set media block size failed: %d: %s\n",
            rc, strerror (rc));

  if (close (fs->device) < 0)
  {
    rc = errno;
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CLOSE))
      printf ("rtems-rfs: buffer-close: file close failed: %d: %s\n",
              rc, strerror (rc));
  }

  return rc;
}

int
rtems_rfs_buffer_sync (rtems_rfs_file_system* fs)
{
  int result = 0;
#if RTEMS_RFS_USE_LIBBLOCK
  rtems_status_code sc;
#endif

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SYNC))
    printf ("rtems-rfs: buffer-sync: syncing\n");

  /*
   * @todo Split in the separate files for each type.
   */
#if RTEMS_RFS_USE_LIBBLOCK
  sc = rtems_bdbuf_syncdev (rtems_rfs_fs_device (fs));
  if (sc != RTEMS_SUCCESSFUL)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SYNC))
      printf ("rtems-rfs: buffer-sync: device sync failed: %s\n",
              rtems_status_text (sc));
    result = EIO;
  }
  rtems_disk_release (fs->disk);
#else
  if (fsync (fs->device) < 0)
  {
    result = errno;
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CLOSE))
      printf ("rtems-rfs: buffer-sync: file sync failed: %d: %s\n",
              result, strerror (result));
  }
#endif
  return result;
}

int
rtems_rfs_buffer_setblksize (rtems_rfs_file_system* fs, size_t size)
{
  int rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SETBLKSIZE))
    printf ("rtems-rfs: buffer-setblksize: block size: %zu\n", size);

  rc = rtems_rfs_buffers_release (fs);
  if ((rc > 0) && rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SETBLKSIZE))
    printf ("rtems-rfs: buffer-setblksize: buffer release failed: %d: %s\n",
            rc, strerror (rc));

  rc = rtems_rfs_buffer_sync (fs);
  if ((rc > 0) && rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_SETBLKSIZE))
    printf ("rtems-rfs: buffer-setblksize: device sync failed: %d: %s\n",
            rc, strerror (rc));

#if RTEMS_RFS_USE_LIBBLOCK
  rc = fs->disk->ioctl (fs->disk, RTEMS_BLKIO_SETBLKSIZE, &size);
  if (rc < 0)
    rc = errno;
#endif
  return rc;
}

static int
rtems_rfs_release_chain (rtems_chain_control* chain,
                         uint32_t*            count,
                         bool                 modified)
{
  rtems_rfs_buffer* buffer;
  int               rrc = 0;
  int               rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_CHAINS))
    printf ("rtems-rfs: release-chain: count=%" PRIu32 "\n", *count);

  while (!rtems_chain_is_empty (chain))
  {
    buffer = (rtems_rfs_buffer*) rtems_chain_get (chain);
    (*count)--;

    buffer->user = (void*) 0;

    rc = rtems_rfs_buffer_io_release (buffer, modified);
    if ((rc > 0) && (rrc == 0))
      rrc = rc;
  }
  return rrc;
}

int
rtems_rfs_buffers_release (rtems_rfs_file_system* fs)
{
  int rrc = 0;
  int rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_BUFFER_RELEASE))
    printf ("rtems-rfs: buffers-release: active:%" PRIu32 " "
            "release:%" PRIu32 " release-modified:%" PRIu32 "\n",
            fs->buffers_count, fs->release_count, fs->release_modified_count);

  rc = rtems_rfs_release_chain (&fs->release,
                                &fs->release_count,
                                false);
  if ((rc > 0) && (rrc == 0))
    rrc = rc;
  rc = rtems_rfs_release_chain (&fs->release_modified,
                                &fs->release_modified_count,
                                true);
  if ((rc > 0) && (rrc == 0))
    rrc = rc;

  return rrc;
}
