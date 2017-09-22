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

#include <sys/cpuset.h>

#include <strings.h>

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

RTEMS_INLINE_ROUTINE void _Processor_mask_Fill( Processor_mask *mask )
{
  BIT_FILL( CPU_MAXIMUM_PROCESSORS, mask );
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
 * @brief Returns true if the processor sets a and b are equal, and false
 * otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_equal(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return !BIT_CMP( CPU_MAXIMUM_PROCESSORS, a, b );
}

/**
 * @brief Returns true if the intersection of the processor sets a and b is
 * non-empty, and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Has_overlap(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return BIT_OVERLAP( CPU_MAXIMUM_PROCESSORS, a, b );
}

/**
 * @brief Returns true if the processor set small is a subset of processor set
 * big, and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_subset(
  const Processor_mask *big,
  const Processor_mask *small
)
{
  return BIT_SUBSET( CPU_MAXIMUM_PROCESSORS, big, small );
}

/**
 * @brief Performs a bitwise a = b & c.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_And(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  BIT_AND2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Performs a bitwise a = b & ~c.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Nand(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  BIT_NAND2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Performs a bitwise a = b | c.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Or(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  BIT_OR2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Performs a bitwise a = b ^ c.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Xor(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  BIT_XOR2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

RTEMS_INLINE_ROUTINE uint32_t _Processor_mask_Count( const Processor_mask *a )
{
  return (uint32_t) BIT_COUNT( CPU_MAXIMUM_PROCESSORS, a );
}

RTEMS_INLINE_ROUTINE uint32_t _Processor_mask_Find_last_set( const Processor_mask *a )
{
  return (uint32_t) BIT_FLS( CPU_MAXIMUM_PROCESSORS, a );
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

/**
 * @brief Creates a processor set from an unsigned 32-bit integer relative to
 * the specified index.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_From_uint32_t(
  Processor_mask *mask,
  uint32_t        bits,
  uint32_t        index
)
{
  _Processor_mask_Zero( mask );
  mask->__bits[ __bitset_words( index ) ] = ((long) bits) << (32 * (index % _BITSET_BITS) / 32);
}

/**
 * @brief Creates a processor set from the specified index.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_From_index(
  Processor_mask *mask,
  uint32_t        index
)
{
  BIT_SETOF( CPU_MAXIMUM_PROCESSORS, (int) index, mask );
}

typedef enum {
  PROCESSOR_MASK_COPY_LOSSLESS,
  PROCESSOR_MASK_COPY_PARTIAL_LOSS,
  PROCESSOR_MASK_COPY_COMPLETE_LOSS,
  PROCESSOR_MASK_COPY_INVALID_SIZE
} Processor_mask_Copy_status;

RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_at_most_partial_loss(
  Processor_mask_Copy_status status
)
{
  return (unsigned int) status <= PROCESSOR_MASK_COPY_PARTIAL_LOSS;
}

Processor_mask_Copy_status _Processor_mask_Copy(
  long       *dst,
  size_t      dst_size,
  const long *src,
  size_t      src_size
);

RTEMS_INLINE_ROUTINE Processor_mask_Copy_status _Processor_mask_To_cpu_set_t(
  const Processor_mask *src,
  size_t                dst_size,
  cpu_set_t            *dst
)
{
  return _Processor_mask_Copy(
    &dst->__bits[ 0 ],
    dst_size,
    &src->__bits[ 0 ],
    sizeof( *src )
  );
}

RTEMS_INLINE_ROUTINE Processor_mask_Copy_status _Processor_mask_From_cpu_set_t(
  Processor_mask  *dst,
  size_t           src_size,
  const cpu_set_t *src
)
{
  return _Processor_mask_Copy(
    &dst->__bits[ 0 ],
    sizeof( *dst ),
    &src->__bits[ 0 ],
    src_size
  );
}

extern const Processor_mask _Processor_mask_The_one_and_only;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PROCESSORMASK_H */
