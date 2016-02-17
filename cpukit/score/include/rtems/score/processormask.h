/**
 * @file
 *
 * @brief Processor Mask API
 *
 * @ingroup ScoreProcessorMask
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#define PROCESSOR_MASK_BITS_PER_FIELD 32

#define PROCESSOR_MASK_FIELD_COUNT \
  ( ( CPU_MAXIMUM_PROCESSORS + PROCESSOR_MASK_BITS_PER_FIELD - 1 ) \
    / PROCESSOR_MASK_BITS_PER_FIELD )

#define PROCESSOR_MASK_BIT( index ) \
  (1UL << ( ( index ) % PROCESSOR_MASK_BITS_PER_FIELD ) )

#define PROCESSOR_MASK_FIELD( index ) \
  ( ( index ) / PROCESSOR_MASK_BITS_PER_FIELD )

/**
 * @brief A bit map consisting of 32-bit integer fields which is large enough
 * to provide one bit for each processor in the system.
 *
 * Processor 0 corresponds to the bit 0 (least-significant) of the field 0 in
 * the array, and so on.
 */
typedef uint32_t Processor_mask[ PROCESSOR_MASK_FIELD_COUNT ];

RTEMS_INLINE_ROUTINE void _Processor_mask_Set( Processor_mask mask, uint32_t index )
{
  mask[ PROCESSOR_MASK_FIELD( index ) ] |= PROCESSOR_MASK_BIT( index );
}

RTEMS_INLINE_ROUTINE void _Processor_mask_Clear( Processor_mask mask, uint32_t index )
{
  mask[ PROCESSOR_MASK_FIELD( index ) ] &= ~PROCESSOR_MASK_BIT( index );
}

RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_set(
  const Processor_mask mask,
  uint32_t index
)
{
  return ( mask[ PROCESSOR_MASK_FIELD( index ) ]
    & PROCESSOR_MASK_BIT( index ) ) != 0;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PROCESSORMASK_H */
