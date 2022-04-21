/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Includes endian information about the target
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

#ifndef _RTEMS_ENDIAN_H
#define _RTEMS_ENDIAN_H

#include <stdint.h>

#include <machine/endian.h>

#ifndef _BYTEORDER_FUNC_DEFINED
#define	_BYTEORDER_FUNC_DEFINED
#define	htonl(x)	__htonl(x)
#define	htons(x)	__htons(x)
#define	ntohl(x)	__ntohl(x)
#define	ntohs(x)	__ntohs(x)
#endif

#define NTOHS(x) (x) = ntohs(x)
#define HTONS(x) (x) = htons(x)
#define NTOHL(x) (x) = ntohl(x)
#define HTONL(x) (x) = htonl(x)

static inline uint16_t rtems_uint16_from_little_endian( const uint8_t *data)
{
  uint16_t value = 0;
  int i;

  for (i = 1; i >= 0; --i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_little_endian( const uint8_t *data)
{
  uint32_t value = 0;
  int i;

  for (i = 3; i >= 0; --i) {
    value = (value << 8) + data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_little_endian( const uint8_t *data)
{
  uint64_t value = 0;
  int i;

  for (i = 7; i >= 0; --i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline uint16_t rtems_uint16_from_big_endian( const uint8_t *data)
{
  uint16_t value = 0;
  int i;

  for (i = 0; i < 2; ++i) {
    value = (uint16_t) ((value << 8) + data [i]);
  }

  return value;
}

static inline uint32_t rtems_uint32_from_big_endian( const uint8_t *data)
{
  uint32_t value = 0;
  int i;

  for (i = 0; i < 4; ++i) {
    value = (value << 8) + (uint32_t) data [i];
  }

  return value;
}

static inline uint64_t rtems_uint64_from_big_endian( const uint8_t *data)
{
  uint64_t value = 0;
  int i;

  for (i = 0; i < 8; ++i) {
    value = (value << 8) + (uint64_t) data [i];
  }

  return value;
}

static inline void rtems_uint16_to_little_endian( uint16_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 2; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_little_endian( uint32_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 4; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_little_endian( uint64_t value, uint8_t *data)
{
  int i;

  for (i = 0; i < 8; ++i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint16_to_big_endian( uint16_t value, uint8_t *data)
{
  int i;

  for (i = 1; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint32_to_big_endian( uint32_t value, uint8_t *data)
{
  int i;

  for (i = 3; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

static inline void rtems_uint64_to_big_endian( uint64_t value, uint8_t *data)
{
  int i;

  for (i = 7; i >= 0; --i) {
    data [i] = (uint8_t) value;
    value >>= 8;
  }
}

#endif /* _RTEMS_ENDIAN_H */
