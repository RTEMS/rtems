/**
 * @file
 *
 * @brief Processor Mask Implementation
 *
 * @ingroup RTEMSScoreProcessorMask
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
