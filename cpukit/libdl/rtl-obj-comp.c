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
 * @brief RTEMS Run-Time Linker Object Compression manages a compress
 *        stream of data.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <rtems/inttypes.h>

#include <rtems/rtl/rtl-allocator.h>
#include <rtems/rtl/rtl-obj-comp.h>
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>

#include "fastlz.h"

#include <stdio.h>

bool
rtems_rtl_obj_comp_open (rtems_rtl_obj_comp* comp,
                         size_t              size)
{
  comp->cache  = NULL;
  comp->fd = -1;
  comp->compression = RTEMS_RTL_COMP_LZ77;
  comp->offset = 0;
  comp->size   = size;
  comp->level  = 0;
  comp->buffer = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, size, false);
  if (!comp->buffer)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for compressor buffer");
    return false;
  }
  comp->read = 0;
  return true;
}

void
rtems_rtl_obj_comp_close (rtems_rtl_obj_comp* comp)
{
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, comp->buffer);
  comp->cache = NULL;
  comp->fd = -1;
  comp->compression = RTEMS_RTL_COMP_LZ77;
  comp->level = 0;
  comp->size = 0;
  comp->offset = 0;
  comp->read = 0;
}

void
rtems_rtl_obj_comp_set (rtems_rtl_obj_comp*  comp,
                        rtems_rtl_obj_cache* cache,
                        int                  fd,
                        int                  compression,
                        off_t                offset)
{
  comp->cache = cache;
  comp->fd = fd;
  comp->compression = compression;
  comp->offset = offset;
  comp->level = 0;
  comp->read = 0;
}

bool
rtems_rtl_obj_comp_read (rtems_rtl_obj_comp* comp,
                         void*               buffer,
                         size_t              length)
{
  uint8_t* bin = buffer;

  if (!comp->cache)
  {
    rtems_rtl_set_error (EINVAL, "not open");
    return false;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
    printf ("rtl:  comp: %2d: fd=%d length=%zu level=%u offset=%" PRIdoff_t " area=[%"
            PRIdoff_t ",%" PRIdoff_t "] read=%" PRIu32 " size=%zu\n",
            comp->fd, comp->cache->fd, length, comp->level, comp->offset,
            comp->offset, comp->offset + length,
            comp->read, comp->size);

  if (comp->fd != comp->cache->fd)
  {
    comp->level = 0;
  }

  while (length)
  {
    size_t buffer_level;

    buffer_level = length > comp->level ? comp->level : length;

    if (buffer_level)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
        printf ("rtl:  comp: copy: length=%zu\n",
                buffer_level);

      memcpy (bin, comp->buffer, buffer_level);

      if ((comp->level - buffer_level) != 0)
      {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
          printf ("rtl:  comp: copy-down: level=%u length=%zu\n",
                  comp->level, comp->level - buffer_level);

        memmove (comp->buffer,
                 comp->buffer + buffer_level,
                 comp->level - buffer_level);
      }

      bin += buffer_level;
      length -= buffer_level;
      comp->level -= buffer_level;
      comp->read += buffer_level;
    }

    if (length)
    {
      uint8_t* input = NULL;
      uint16_t block_size;
      size_t   in_length = sizeof (block_size);
      int      decompressed;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
        printf ("rtl:  comp: read block-size: offset=%" PRIdoff_t "\n",
                comp->offset);

      if (!rtems_rtl_obj_cache_read (comp->cache, comp->fd, comp->offset,
                                     (void**) &input, &in_length))
        return false;

      block_size = (input[0] << 8) | input[1];

      comp->offset += sizeof (block_size);

      in_length = block_size;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
        printf ("rtl:  comp: read block: offset=%" PRIdoff_t " size=%u\n",
                comp->offset, block_size);

      if (!rtems_rtl_obj_cache_read (comp->cache, comp->fd, comp->offset,
                                     (void**) &input, &in_length))
        return false;

      if (in_length != block_size)
      {
        rtems_rtl_set_error (EIO, "compressed read failed: bs=%u in=%zu",
                             block_size, in_length);
        return false;
      }

      switch (comp->compression)
      {
        case RTEMS_RTL_COMP_NONE:
          memcpy (comp->buffer, input, in_length);
          decompressed = in_length;
          break;

        case RTEMS_RTL_COMP_LZ77:
          decompressed = fastlz_decompress (input, in_length,
                                            comp->buffer, comp->size);
          if (decompressed == 0)
          {
            rtems_rtl_set_error (EBADF, "decompression failed");
            return false;
          }
          break;

        default:
          rtems_rtl_set_error (EINVAL, "bad compression type");
          return false;
      }

      comp->offset += block_size;

      comp->level = decompressed;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_COMP))
        printf ("rtl:  comp: expand: offset=%" PRIdoff_t \
                " level=%u read=%" PRIu32 "\n",
                comp->offset, comp->level, comp->read);
    }
  }

  return true;
}
