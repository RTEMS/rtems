/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Author: Jeffrey O. Hill
 *
 * Copyright 2012. Los Alamos National Security, LLC.
 * This material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL),
 * which is operated by Los Alamos National Security, LLC for
 * the U.S. Department of Energy. The U.S. Government has rights
 * to use, reproduce, and distribute this software.  NEITHER THE
 * GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY
 * WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR
 * THE USE OF THIS SOFTWARE.
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

#ifndef _NIOS2_COUNT_ZEROS_H
#define _NIOS2_COUNT_ZEROS_H

#include <stdint.h>

#include <rtems/score/bitfield.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * This implementation is currently much more efficient than
 * the GCC provided __builtin_clz
 */
static inline unsigned _Nios2_Count_leading_zeros( uint32_t p )
{
  unsigned bitIdx;

  if ( p <= 0xffffu ) {
    if ( p < 0x100u ) {
      bitIdx = _Bitfield_Leading_zeros[ p ] + 24u;
    } else {
      bitIdx = _Bitfield_Leading_zeros[ p >> 8u ] + 16u;
    }
  } else {
    p >>= 16u;

    if ( p < 0x100u ) {
      bitIdx = _Bitfield_Leading_zeros[ p ] + 8u;
    } else {
      bitIdx = _Bitfield_Leading_zeros[ p >> 8u ];
    }
  }

  return bitIdx;
}

/*
 * This implementation is currently much more efficient than
 * the GCC provided __builtin_ctz
 */
static inline unsigned _Nios2_Count_trailing_zeros( uint32_t p )
{
  return 31u - _Nios2_Count_leading_zeros( p & ( -p ) );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NIOS2_COUNT_ZEROS_H */
