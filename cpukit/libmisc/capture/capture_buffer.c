/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Performance Monitoring and Measurement Framework.
 *
 * This is the Target Interface Command Line Interface. You need
 * to start the RTEMS monitor.
 */

/*
 *  COPYRIGHT (c) 2014.  On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <rtems/score/assert.h>
#include "capture_buffer.h"

void*
rtems_capture_buffer_allocate (rtems_capture_buffer* buffer, size_t size)
{
  void* ptr = NULL;

  if ((buffer->count + size) <= buffer->end)
  {
    size_t end;

    /*
     * Determine if the end of free space is marked with the end of buffer
     * space, or the head of allocated space.
     *
     * |...|head| freespace |tail| ...| end
     *
     * tail|.....|head| freespace| end
     */
    if (buffer->tail > buffer->head)
    {
      end = buffer->tail;
    } else
    {
      end = buffer->end;
    }

    /*
     * Can we allocate it easily?
     */
    if ((buffer->head + size) <= end)
    {
      ptr = &buffer->buffer[buffer->head];
      buffer->head += size;
      buffer->count = buffer->count + size;
      if (buffer->max_rec < size)
        buffer->max_rec = size;
    }
    else
    {
      /*
       * We have to consider wrapping around to the front of the buffer
       *
       * If there is no room at the end of the buffer and we have we already
       * wrapped then we can't allocate and if there is room at the front of
       * the buffer.
       */
      if ((end != buffer->tail) && (buffer->tail >= size))
      {
        /*
         * Change the end pointer to the last used byte, so a read will wrap
         * when out of data
         */
        buffer->end = buffer->head;

        /*
         * Now return the buffer
         */
        ptr = buffer->buffer;
        buffer->head = size;
        buffer->count = buffer->count + size;
        if (buffer->max_rec < size)
          buffer->max_rec = size;
      }
    }
  }

  return ptr;
}

void*
rtems_capture_buffer_free (rtems_capture_buffer* buffer, size_t size)
{
  void*  ptr;
  size_t next;
  size_t buff_size;

  if (size == 0)
    return NULL;

  ptr = rtems_capture_buffer_peek (buffer, &buff_size);
  next = buffer->tail + size;

  /*
   * Check if we are freeing space past the end of the buffer
   */
  _Assert (! rtems_capture_buffer_is_empty( buffer));
  _Assert (!((buffer->tail > buffer->head) && (next > buffer->end)));
  _Assert (!((buffer->tail < buffer->head) && (next > buffer->head)));

  buffer->count = buffer->count - size;

  if (next == buffer->end)
  {
    buffer->end = buffer->size;
    buffer->tail = 0;
  } else
  {
    buffer->tail = next;
  }

  return ptr;
}
