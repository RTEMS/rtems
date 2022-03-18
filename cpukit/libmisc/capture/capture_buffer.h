/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Capture buffer
 *
 * This is a set of functions to control a variable length capture
 * record buffer.
 */

/*
 * COPYRIGHT (c) 2014.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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

#ifndef __CAPTURE_BUFFER_H_
#define __CAPTURE_BUFFER_H_

#include <stdlib.h>

/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Capture buffer. There is one per CPU.
 */
typedef struct rtems_capture_buffer {
  uint8_t* buffer;       /**< The per cpu buffer. */
  size_t   size;         /**< The size of the buffer in bytes. */
  size_t   count;        /**< The number of used bytes in the buffer. */
  size_t   head;         /**< First record. */
  size_t   tail;         /**< Head == Tail for empty. */
  size_t   end;          /**< Buffer current end, it may move in. */
  size_t   max_rec;      /**< The largest record in the buffer. */
} rtems_capture_buffer;

static inline void
rtems_capture_buffer_flush (rtems_capture_buffer* buffer)
{
  buffer->end = buffer->size;
  buffer->head = buffer->tail =  0;
  buffer->count = 0;
  buffer->max_rec = 0;
}

static inline void
rtems_capture_buffer_create (rtems_capture_buffer* buffer, size_t size)
{
  buffer->buffer = malloc(size);
  buffer->size = size;
  rtems_capture_buffer_flush (buffer);
}

static inline void
rtems_capture_buffer_destroy (rtems_capture_buffer*  buffer)
{
  rtems_capture_buffer_flush (buffer);
  free (buffer->buffer);
  buffer->buffer = NULL;
}

static inline bool
rtems_capture_buffer_is_empty (rtems_capture_buffer* buffer)
{
   return buffer->count == 0;
}

static inline bool
rtems_capture_buffer_is_full (rtems_capture_buffer* buffer)
{
   return buffer->count == buffer->size;
}

static inline bool
rtems_capture_buffer_has_wrapped (rtems_capture_buffer* buffer)
{
  if (buffer->tail > buffer->head)
    return true;

  return false;
}

static inline void*
rtems_capture_buffer_peek (rtems_capture_buffer* buffer, size_t* size)
{
  if (rtems_capture_buffer_is_empty (buffer))
  {
    *size = 0;
    return NULL;
  }

  if (buffer->tail > buffer->head)
    *size = buffer->end - buffer->tail;
  else
    *size = buffer->head - buffer->tail;

  return &buffer->buffer[buffer->tail];
}

void* rtems_capture_buffer_allocate (rtems_capture_buffer* buffer, size_t size);

void* rtems_capture_buffer_free (rtems_capture_buffer* buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
