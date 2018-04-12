/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <rtems/inttypes.h>

#include <rtems/rtl/rtl-allocator.h>
#include <rtems/rtl/rtl-obj-cache.h>
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>

bool
rtems_rtl_obj_cache_open (rtems_rtl_obj_cache* cache, size_t size)
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
rtems_rtl_obj_cache_close (rtems_rtl_obj_cache* cache)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
    printf ("rtl: cache: %2d: close\n", cache->fd);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, cache->buffer);
  cache->buffer    = NULL;
  cache->fd        = -1;
  cache->file_size = 0;
  cache->level     = 0;
}

void
rtems_rtl_obj_cache_flush (rtems_rtl_obj_cache* cache)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
    printf ("rtl: cache: %2d: flush\n", cache->fd);
  cache->fd        = -1;
  cache->file_size = 0;
  cache->offset    = 0;
  cache->level     = 0;
}

bool
rtems_rtl_obj_cache_read (rtems_rtl_obj_cache* cache,
                          int                  fd,
                          off_t                offset,
                          void**               buffer,
                          size_t*              length)
{
  struct stat sb;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
    printf ("rtl: cache: %2d: fd=%d offset=%" PRIdoff_t "length=%zu area=[%"
            PRIdoff_t ",%" PRIdoff_t "] cache=[%" PRIdoff_t ",%" PRIdoff_t "] size=%zu\n",
            fd, cache->fd, offset, *length,
            offset, offset + *length,
            cache->offset, cache->offset + cache->level,
            cache->file_size);

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

    /*
     * We sometimes are asked to read strings of a length we do not know.
     */
    if ((offset + *length) > cache->file_size)
    {
      *length = cache->file_size - offset;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
        printf ("rtl: cache: %2d: truncate length=%d\n", fd, (int) *length);
    }
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
       * Do not read past the end of the file.
       */
      if ((offset + buffer_read) > cache->file_size)
        buffer_read = cache->file_size - offset;

      /*
       * Is any part of the data in the cache ?
       */
      if ((offset >= cache->offset) &&
          (offset < (cache->offset + cache->level)))
      {
        size_t size;

        buffer_offset = offset - cache->offset;
        size          = cache->level - buffer_offset;

        /*
         * Return the location of the data in the cache.
         */
        *buffer = cache->buffer + buffer_offset;

        /*
         * Is all the data in the cache or just a part ?
         */
        if (*length <= size)
          return true;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
          printf ("rtl: cache: %2d: copy-down: buffer_offset=%d size=%d level=%d\n",
                  fd, (int) buffer_offset, (int) size, (int) cache->level);

        /*
         * Copy down the data in the buffer and then fill the remaining space
         * with as much data we are able to read.
         */
        memmove (cache->buffer, cache->buffer + buffer_offset, size);

        cache->offset = offset;
        cache->level  = size;
        buffer_read   = cache->size - cache->level;
        buffer_offset = size;

        /*
         * Do not read past the end of the file.
         */
        if ((offset + buffer_offset + buffer_read) > cache->file_size)
          buffer_read = cache->file_size - (offset + buffer_offset);
      }
    }

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
      printf ("rtl: cache: %2d: seek: offset=%" PRIdoff_t "buffer_offset=%zu"
              "read=%zu cache=[%" PRIdoff_t ",%" PRIdoff_t "] "
              "dist=%" PRIdoff_t "\n",
              fd, offset + buffer_offset, buffer_offset, buffer_read,
              offset, offset + buffer_read,
              (cache->file_size - offset));

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

    cache->level = buffer_offset + buffer_read;

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
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_CACHE))
          printf ("rtl: cache: %2d: read: past end by=%d\n", fd, (int) buffer_read);
        cache->level = cache->level - buffer_read;
        buffer_read = 0;
      }
      else
      {
        buffer_read -= r;
        buffer_offset += r;
      }
    }

    cache->offset = offset;

    if (cache->fd != fd)
    {
      cache->fd = fd;

      if (fstat (cache->fd, &sb) < 0)
      {
        rtems_rtl_set_error (errno, "file stat failed");
        return false;
      }

      cache->file_size = sb.st_size;
    }
  }

  return false;
}

bool
rtems_rtl_obj_cache_read_byval (rtems_rtl_obj_cache* cache,
                                int                  fd,
                                off_t                offset,
                                void*                buffer,
                                size_t               length)
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
