/**
 * @file
 *
 * @brief Processor Mask API
 *
 * @ingroup ScoreProcessorMask
 */

/*
 * Copyright (c) 2016, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_PROCESSORMASK_H
#define _RTEMS_SCORE_PROCESSORMASK_H

#include <rtems/score/cpu.h>

#include <sys/_bitset.h>
#include <sys/bitset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreProcessorMask Processor Mask
 *
 * @ingroup Score
 *
 * The processor mask provides a bit map large enough to provide one bit for
 * each processor in the system.  It is a fixed size internal data type
 * provided for efficiency in addition to the API level cpu_set_t.
 *
 * @{
 */

/**
 * @brief A bit map which is large enough to provide one bit for each processor
 * in the system.
 */
typedef BITSET_DEFINE( Processor_mask, CPU_MAXIMUM_PROCESSORS ) Processor_mask;

RTEMS_INLINE_ROUTINE void _Processor_mask_Zero( Processor_mask *mask )
{
  BIT_ZERO( CPU_MAXIMUM_PROCESSORS, mask );
}

RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_zero( const Processor_mask *mask )
{
  return BIT_EMPTY( CPU_MAXIMUM_PROCESSORS, mask );
}

RTEMS_INLINE_ROUTINE void _Processor_mask_Assign(
  Processor_mask *dst, const Processor_mask *src
)
{
  BIT_COPY( CPU_MAXIMUM_PROCESSORS, src, dst );
}

RTEMS_INLINE_ROUTINE void _Processor_mask_Set(
  Processor_mask *mask,
  uint32_t        index
)
{
  BIT_SET( CPU_MAXIMUM_PROCESSORS, index, mask );
}

RTEMS_INLINE_ROUTINE void _Processor_mask_Clear(
  Processor_mask *mask,
  uint32_t        index
)
{
  BIT_CLR( CPU_MAXIMUM_PROCESSORS, index, mask );
}

RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_set(
  const Processor_mask *mask,
  uint32_t              index
)
{
  return BIT_ISSET( CPU_MAXIMUM_PROCESSORS, index, mask );
}

/**
 * @brief Returns the subset of 32 processors containing the specified index as
 * an unsigned 32-bit integer.
 */
RTEMS_INLINE_ROUTINE uint32_t _Processor_mask_To_uint32_t(
  const Processor_mask *mask,
  uint32_t              index
)
{
  long bits = mask->__bits[ __bitset_words( index ) ];

  return (uint32_t) (bits >> (32 * (index % _BITSET_BITS) / 32));
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PROCESSORMASK_H */
