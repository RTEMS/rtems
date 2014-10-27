/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object File cache buffers a section of the
 *        object file in a buffer to localise read performance.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <rtems/rtl/rtl-allocator.h>
#include "rtl-obj-cache.h"
#include "rtl-error.h"

bool
rtems_rtl_obj_cache_open (rtems_rtl_obj_cache_t* cache, size_t size)
{
  cache->fd        = -1;
  cache->file_size = 0;
  cache->offset    = 0;
  cache->size      = size;
  cache->level     = 0;
  cache->buffer    = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, size, false);
  if (!cache->buffer)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for cache buffer");
    return false;
  }
  return true;
}

void
rtems_rtl_obj_cache_close (rtems_rtl_obj_cache_t* cache)
{
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, cache->buffer);
  cache->fd        = -1;
  cache->file_size = 0;
  cache->level     = 0;
}

void
rtems_rtl_obj_cache_flush (rtems_rtl_obj_cache_t* cache)
{
  cache->fd        = -1;
  cache->file_size = 0;
  cache->level     = 0;
}

bool
rtems_rtl_obj_cache_read (rtems_rtl_obj_cache_t* cache,
                          int                    fd,
                          off_t                  offset,
                          void**                 buffer,
                          size_t*                length)
{
  struct stat sb;

  if (*length > cache->size)
  {
    rtems_rtl_set_error (EINVAL, "read size larger than cache size");
    return false;
  }

  if (cache->fd == fd)
  {
    if (offset > cache->file_size)
    {
      rtems_rtl_set_error (EINVAL, "offset past end of file: offset=%i size=%i",
                           (int) offset, (int) cache->file_size);
      return false;
    }

    if ((offset + *length) > cache->file_size)
      *length = cache->file_size - offset;
  }

  while (true)
  {
    size_t buffer_offset = 0;
    size_t buffer_read = cache->size;

    /*
     * Is the data in the cache for this file ?
     */
    if (fd == cache->fd)
    {
      /*
       * Is any part of the data in the cache ?
       */
      if ((offset >= cache->offset) &&
          (offset < (cache->offset + cache->level)))
      {
        buffer_offset = offset - cache->offset;

        /*
         * Return the location of the data in the cache.
         */
        *buffer = cache->buffer + buffer_offset;

        /*
         * Is all the data in the cache or just a part ?
         */
        if (*length <= (cache->level - buffer_offset))
        {
          return true;
        }

        /*
         * Copy down the data in the buffer and then fill the remaining
         * space with as much data we are able to read.
         */
        memmove (cache->buffer,
                 cache->buffer + buffer_offset,
                 cache->size - buffer_offset);

        buffer_read   = buffer_offset;
        buffer_offset = cache->size - buffer_offset;
      }
    }

    if (lseek (fd, offset + buffer_offset, SEEK_SET) < 0)
    {
      rtems_rtl_set_error (errno, "file seek failed");
      return false;
    }

    /*
     * Loop reading the data from the file until either an error or 0 is
     * returned and if data has been read check if the amount is what we
     * want. If not it is an error. A POSIX read can read data in fragments.
     */
    cache->level = buffer_read;
    while (buffer_read)
    {
      int r = read (fd, cache->buffer + buffer_offset, buffer_read);
      if (r < 0)
      {
        rtems_rtl_set_error (errno, "file read failed");
        return false;
      }
      if ((r == 0) && buffer_read)
      {
        cache->level = cache->level - buffer_read;
        buffer_read = 0;
      }
      else
      {
        buffer_read -= r;
        buffer_offset += r;
      }
    }

    cache->fd = fd;
    cache->offset = offset;

    if (fstat (cache->fd, &sb) < 0)
    {
      rtems_rtl_set_error (errno, "file stat failed");
      return false;
    }

    cache->file_size = sb.st_size;
  }

  return false;
}

bool
rtems_rtl_obj_cache_read_byval (rtems_rtl_obj_cache_t* cache,
                                int                    fd,
                                off_t                  offset,
                                void*                  buffer,
                                size_t                 length)
{
  void*  cbuffer = 0;
  size_t len = length;
  bool   ok = rtems_rtl_obj_cache_read (cache, fd, offset, &cbuffer, &len);
  if (ok && (len != length))
    ok = false;
  if (ok)
    memcpy (buffer, cbuffer, length);
  return ok;
}
