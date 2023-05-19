/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProcessorMask
 *
 * @brief This source file contains the definition of
 *   ::_Processor_mask_The_one_and_only and the implementation of
 *   _Processor_mask_Copy().
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/processormask.h>

const Processor_mask _Processor_mask_The_one_and_only = { .__bits[ 0 ] = 1 };

Processor_mask_Copy_status _Processor_mask_Copy(
  long       *dst,
  size_t      dst_size,
  const long *src,
  size_t      src_size
)
{
  long inclusive = 0;
  long exclusive = 0;

  if ( ( dst_size | src_size ) % sizeof( long ) != 0 ) {
    return PROCESSOR_MASK_COPY_INVALID_SIZE;
  }

  while ( dst_size > 0 && src_size > 0 ) {
    long bits = *src;

    inclusive |= bits;
    *dst = bits;
    ++dst;
    ++src;
    dst_size -= sizeof( long );
    src_size -= sizeof( long );
  }

  while ( dst_size > 0 ) {
    *dst = 0;
    ++dst;
    dst_size -= sizeof( long );
  }

  while ( src_size > 0 ) {
    exclusive |= *src;
    ++src;
    src_size -= sizeof( long );
  }

  if ( exclusive != 0 ) {
    if ( inclusive != 0 ) {
      return PROCESSOR_MASK_COPY_PARTIAL_LOSS;
    } else {
      return PROCESSOR_MASK_COPY_COMPLETE_LOSS;
    }
  }

  return PROCESSOR_MASK_COPY_LOSSLESS;
}
