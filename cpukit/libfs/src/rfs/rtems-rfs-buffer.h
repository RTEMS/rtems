/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief Maps Blocks to the Media Interface Layers
 *
 * These functions map blocks to the media interface layers.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#if !defined (_RTEMS_RFS_BUFFER_H_)
#define _RTEMS_RFS_BUFFER_H_

#include <errno.h>

#include <rtems/rfs/rtems-rfs-file-system-fwd.h>
#include <rtems/rfs/rtems-rfs-trace.h>

/**
 * Define the method used to interface to the buffers. It can be libblock or
 * device I/O. The libblock interface is to the RTEMS cache and block devices
 * and device I/O accesses the media via a device file handle.
 */
#if defined (__rtems__)
#define RTEMS_RFS_USE_LIBBLOCK 1
#endif

/**
 * The RTEMS RFS I/O Layering.
 */
#if RTEMS_RFS_USE_LIBBLOCK
#include <rtems/bdbuf.h>
#include <rtems/error.h>

typedef rtems_blkdev_bnum  rtems_rfs_buffer_block;
typedef rtems_bdbuf_buffer rtems_rfs_buffer;
#define rtems_rfs_buffer_io_request rtems_rfs_buffer_bdbuf_request
#define rtems_rfs_buffer_io_release rtems_rfs_buffer_bdbuf_release

/**
 * Request a buffer from the RTEMS libblock BD buffer cache.
 */
int rtems_rfs_buffer_bdbuf_request (rtems_rfs_file_system* fs,
                                    rtems_rfs_buffer_block block,
                                    bool                   read,
                                    rtems_rfs_buffer**     buffer);
/**
 * Release a buffer to the RTEMS libblock BD buffer cache.
 */
int rtems_rfs_buffer_bdbuf_release (rtems_rfs_buffer* handle,
                                    bool              modified);
#else /* Device I/O */
typedef uint32_t rtems_rfs_buffer_block;
typedef struct _rtems_rfs_buffer
{
  rtems_chain_node       link;
  rtems_rfs_buffer_block user;
  void*                  buffer;
  size_t                 size;
  uint32_t               references;
} rtems_rfs_buffer;
#define rtems_rfs_buffer_io_request rtems_rfs_buffer_deviceio_request
#define rtems_rfs_buffer_io_release rtems_rfs_buffer_deviceio_release

/**
 * Request a buffer from the device I/O.
 */
int rtems_rfs_buffer_deviceio_request (rtems_rfs_file_system* fs,
                                       rtems_rfs_buffer_block block,
                                       bool                   read,
                                       rtems_rfs_buffer*      buffer);
/**
 * Release a buffer to the RTEMS libblock BD buffer cache.
 */
int rtems_rfs_buffer_deviceio_release (rtems_rfs_buffer* handle,
                                       bool              modified);
#endif

/**
 * RFS Buffer handle.
 */
typedef struct rtems_rfs_buffer_handle_t
{
  /**
   * Has the buffer been modifed?
   */
  bool dirty;

  /**
   * Block number. The lower layer block number may be absolute and we maybe
   * relative to an offset in the disk so hold locally.
   */
  rtems_rfs_buffer_block bnum;

  /**
   * Reference the buffer descriptor.
   */
  rtems_rfs_buffer* buffer;

} rtems_rfs_buffer_handle;

/**
 * The buffer linkage.
 */
#define rtems_rfs_buffer_link(_h) (&(_h)->buffer->link)

/**
 * Return the start of the data area of the buffer given a handle.
 */
#define rtems_rfs_buffer_data(_h) ((void*)((_h)->buffer->buffer))

/**
 * Return the size of the buffer given a handle.
 */
#define rtems_rfs_buffer_size(_h) ((_h)->buffer->size)

/**
 * Return the block number.
 */
#define rtems_rfs_buffer_bnum(_h) ((_h)->bnum)

/**
 * Return the buffer dirty status.
 */
#define rtems_rfs_buffer_dirty(_h) ((_h)->dirty)

/**
 * Does the handle have a valid block attached ?
 */
#define rtems_rfs_buffer_handle_has_block(_h) ((_h)->buffer ? true : false)

/**
 * Mark the buffer as dirty.
 */
#define rtems_rfs_buffer_mark_dirty(_h) ((_h)->dirty = true)

/**
 * Return the reference count.
 */
#define rtems_rfs_buffer_refs(_h) ((_h)->buffer->references)

/**
 * Increment the reference count.
 */
#define rtems_rfs_buffer_refs_up(_h) ((_h)->buffer->references += 1)

/**
 * Decrement the reference count.
 */
#define rtems_rfs_buffer_refs_down(_h) ((_h)->buffer->references -= 1)

/**
 * Request a buffer. The buffer can be filled with data from the media
 * (read == true) or you can request a buffer to fill with data.
 *
 * @param[in] fs is the file system data.
 * @param[in] handle is the handle the requested buffer is attached to.
 * @param[in] block is the block number.
 * @param[in] read Read the data from the disk.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_handle_request (rtems_rfs_file_system*   fs,
                                     rtems_rfs_buffer_handle* handle,
                                     rtems_rfs_buffer_block   block,
                                     bool                     read);

/**
 * Release a buffer. If the buffer is dirty the buffer is written to disk. The
 * result does not indicate if the data was successfully written to the disk as
 * this operation may be performed in asynchronously to this release.
 *
 * @param[in] fs is the file system data.
 * @param[in] handle is the handle the requested buffer is attached to.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_handle_release (rtems_rfs_file_system*   fs,
                                     rtems_rfs_buffer_handle* handle);

/**
 * Open a handle.
 *
 * @param[in] fs i the file system data.
 * @param[in] handle i the buffer handle to open.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
static inline int
rtems_rfs_buffer_handle_open (rtems_rfs_file_system*   fs,
                              rtems_rfs_buffer_handle* handle)
{
  handle->dirty = false;
  handle->bnum  = 0;
  handle->buffer = NULL;
  return 0;
}

/**
 * Close a handle.
 *
 * @param[in] fs is the file system data.
 * @param[in] handle is the buffer handle to close.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
static inline int
rtems_rfs_buffer_handle_close (rtems_rfs_file_system*   fs,
                               rtems_rfs_buffer_handle* handle)
{
  rtems_rfs_buffer_handle_release (fs, handle);
  handle->dirty = false;
  handle->bnum  = 0;
  handle->buffer = NULL;
  return 0;
}

/**
 * Open the buffer interface.
 *
 * @param[in] name is a pointer to the device name to the media.
 * @param[in] fs is the file system data.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_open (const char* name, rtems_rfs_file_system* fs);

/**
 * Close the buffer interface.
 *
 * @param[in] fs is the file system data.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_close (rtems_rfs_file_system* fs);

/**
 * Sync all buffers to the media.
 *
 * @param[in] fs is the file system data.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_sync (rtems_rfs_file_system* fs);

/**
 * Set the block size of the device.
 *
 * @param[in] fs is the file system data.
 * @param[in] size is the new block size.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffer_setblksize (rtems_rfs_file_system* fs, size_t size);

/**
 * Release any chained buffers.
 *
 * @param[in] fs is the file system data.
 *
 * @retval 0 Successful operation.
 * @retval error_code An error occurred.
 */
int rtems_rfs_buffers_release (rtems_rfs_file_system* fs);

#endif
