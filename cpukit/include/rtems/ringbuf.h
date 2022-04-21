/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Simple Ring Buffer Functionality
 *
 * This file provides simple ring buffer functionality.
 */

/*
 * COPYRIGHT (C) 1989-1999 On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RINGBUF_H
#define _RTEMS_RINGBUF_H

#include <rtems.h>

#ifndef RINGBUF_QUEUE_LENGTH
#define RINGBUF_QUEUE_LENGTH 128
#endif

typedef struct {
  uint8_t buffer[RINGBUF_QUEUE_LENGTH];
  volatile int  head;
  volatile int  tail;
  rtems_interrupt_lock lock;
} Ring_buffer_t;

#define Ring_buffer_Initialize( _buffer ) \
  do { \
    (_buffer)->head = (_buffer)->tail = 0; \
    rtems_interrupt_lock_initialize(&(_buffer)->lock, "ring buffer"); \
  } while ( 0 )

#define Ring_buffer_Destory( _buffer ) \
  do { \
    rtems_interrupt_lock_destroy(&(_buffer)->lock); \
  } while ( 0 )

#define Ring_buffer_Is_empty( _buffer ) \
   ( (_buffer)->head == (_buffer)->tail )

#define Ring_buffer_Is_full( _buffer ) \
   ( (_buffer)->head == ((_buffer)->tail + 1) % RINGBUF_QUEUE_LENGTH )

#define Ring_buffer_Add_character( _buffer, _ch ) \
  do { \
    rtems_interrupt_lock_context lock_context; \
    \
    rtems_interrupt_lock_acquire( &(_buffer)->lock, &lock_context ); \
      (_buffer)->tail = ((_buffer)->tail+1) % RINGBUF_QUEUE_LENGTH; \
      (_buffer)->buffer[ (_buffer)->tail ] = (_ch); \
    rtems_interrupt_lock_release( &(_buffer)->lock, &lock_context ); \
  } while ( 0 )

#define Ring_buffer_Remove_character( _buffer, _ch ) \
  do { \
    rtems_interrupt_lock_context lock_context; \
    \
    rtems_interrupt_lock_acquire( &(_buffer)->lock, &lock_context ); \
      (_buffer)->head = ((_buffer)->head+1) % RINGBUF_QUEUE_LENGTH; \
      (_ch) = (_buffer)->buffer[ (_buffer)->head ]; \
    rtems_interrupt_lock_release( &(_buffer)->lock, &lock_context ); \
  } while ( 0 )

#endif
