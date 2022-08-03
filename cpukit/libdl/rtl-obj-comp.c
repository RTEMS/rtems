/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object Compression manages a compress
 *        stream of data.
 */

/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
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
    printf ("rtl:  comp: %2d: fd=%d length=%zu level=%zu offset=%" PRIdoff_t " area=[%"
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
          printf ("rtl:  comp: copy-down: level=%zu length=%zu\n",
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
                " level=%zu read=%" PRIu32 "\n",
                comp->offset, comp->level, comp->read);
    }
  }

  return true;
}
