/**
 * @file
 *
 * @brief Processor Mask API
 *
 * @ingroup RTEMSScoreProcessorMask
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
 * @defgroup RTEMSScoreProcessorMask Processor Mask
 *
 * @ingroup RTEMSScore
 *
 * @brief Processor Mask
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

/**
 * @brief Sets the bits of the mask to zero, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] mask The mask to set to zero.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Zero( Processor_mask *mask )
{
  BIT_ZERO( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Checks if the mask is zero, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param mask The mask to check whether is is zero
 *
 * @retval true The mask is zero.
 * @retval false The mask is not zero.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_zero( const Processor_mask *mask )
{
  return BIT_EMPTY( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Fills the mask, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] mask The mask to fill
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Fill( Processor_mask *mask )
{
  BIT_FILL( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Copies the mask to another mask, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] dst The mask to copy @a src to.
 * @param src The mask to copy to @a dst.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Assign(
  Processor_mask *dst, const Processor_mask *src
)
{
  BIT_COPY( CPU_MAXIMUM_PROCESSORS, src, dst );
}

/**
 * @brief Sets the specified index bit of the mask.
 *
 * @param[out] mask The mask to set the bit of.
 * @param index The index of the bit that shall be set.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Set(
  Processor_mask *mask,
  uint32_t        index
)
{
  BIT_SET( CPU_MAXIMUM_PROCESSORS, index, mask );
}

/**
 * @brief Clears the specified index bit of the mask.
 *
 * @param[out] mask The mask to clear the bit of.
 * @param index The index of the bit that shall be cleared.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Clear(
  Processor_mask *mask,
  uint32_t        index
)
{
  BIT_CLR( CPU_MAXIMUM_PROCESSORS, index, mask );
}

/**
 * @brief Checks if the specified index bit of the mask is set.
 *
 * @param mask The mask to check if the specified bit is set.
 * @param index The index of the bit that is checked.
 *
 * @retval true The specified index bit is set.
 * @retval false The specified index bit is not set.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_set(
  const Processor_mask *mask,
  uint32_t              index
)
{
  return BIT_ISSET( CPU_MAXIMUM_PROCESSORS, index, mask );
}

/**
 * @brief Checks if the processor sets a and b are equal.
 *
 * @param a The first processor set.
 * @param b The seconde processor set.
 *
 * @retval true The processor sets a and b are equal.
 * @retval false The processor sets a and b are not equal.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_equal(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return !BIT_CMP( CPU_MAXIMUM_PROCESSORS, a, b );
}

/**
 * @brief Checks if the intersection of the processor sets a and b is
 * non-empty.
 *
 * @param a The first processor set.
 * @param b The second processor set.
 *
 * @retval true The intersection of the processor sets a and b is non-empty.
 * @retval false The intersection of the processor sets a and b is empty.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Has_overlap(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return BIT_OVERLAP( CPU_MAXIMUM_PROCESSORS, a, b );
}

/**
 * @brief Checks if the processor set small is a subset of processor set
 * big.
 *
 * @param big The bigger processor set.
 * @param small The smaller processor set.
 *
 * @retval true @a small is a subset of @a big.
 * @retval false @a small is not a subset of @a big.
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
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the AND-operation.
 * @param c The second parameter of the AND-operation.
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
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the operation.
 * @param c The second parameter of the operation.
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
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the OR-operation.
 * @param c The second parameter of the OR-operation.
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
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the XOR-operation.
 * @param c The second parameter of the XOR-operation.
 */
RTEMS_INLINE_ROUTINE void _Processor_mask_Xor(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  BIT_XOR2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Gets the number of set bits in the processor mask.
 *
 * @param a The processor mask of which the set bits are counted.
 *
 * @return The number of set bits in @a a.
 */
RTEMS_INLINE_ROUTINE uint32_t _Processor_mask_Count( const Processor_mask *a )
{
  return (uint32_t) BIT_COUNT( CPU_MAXIMUM_PROCESSORS, a );
}

/**
 * @brief Finds the last set of the processor mask.
 *
 * @param a The processor mask wo find the last set of.
 *
 * @return The last set of @a a.
 */
RTEMS_INLINE_ROUTINE uint32_t _Processor_mask_Find_last_set( const Processor_mask *a )
{
  return (uint32_t) BIT_FLS( CPU_MAXIMUM_PROCESSORS, a );
}

/**
 * @brief Returns the subset of 32 processors containing the specified index as
 * an unsigned 32-bit integer.
 *
 * @param mask The processor mask.
 * @param index The specified index.
 *
 * @return The subset containing the specified index as an unsigned 32-bit integer.
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
 *
 * @param[out] mask The mask that is created.
 * @param bits The bits for creating the mask.
 * @param index The index to which the mask is relative.
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
 *
 * @param[out] The mask that is created.
 * @param index The specified index.
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

/**
 * @brief Checks if the copy status guarantees at most partial loss.
 *
 * @param status The copy status to check.
 *
 * @retval true At most partial loss can be guaranteed.
 * @retval false The status indicates more than partial loss.
 */
RTEMS_INLINE_ROUTINE bool _Processor_mask_Is_at_most_partial_loss(
  Processor_mask_Copy_status status
)
{
  return (unsigned int) status <= PROCESSOR_MASK_COPY_PARTIAL_LOSS;
}

/**
 * @brief Copies one mask to another.
 *
 * @param[out] dst The destination of the copy operation.
 * @param dst_size The size of @a dst.
 * @param src The source of the copy operation.
 * @param src_size The size of @a src.
 *
 * @retval PROCESSOR_MASK_COPY_LOSSLESS It is guaranteed that the copy
 *      operation is lossless.
 * @retval PROCESSOR_MASK_COPY_PARTIAL_LOSS Partial loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_COMPLETE_LOSS Complete loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_INVALID_SIZE One of the arguments sizes
 *      is invalid (bigger than the size of a long).
 */
Processor_mask_Copy_status _Processor_mask_Copy(
  long       *dst,
  size_t      dst_size,
  const long *src,
  size_t      src_size
);

/**
 * @brief Copies one mask to another.
 *
 * @param src The source for the copy operation.
 * @param dst_size The size of @a dst.
 * @param[out] dst The destination for the copy operation.
 *
 * @retval PROCESSOR_MASK_COPY_LOSSLESS It is guaranteed that the copy
 *      operation is lossless.
 * @retval PROCESSOR_MASK_COPY_PARTIAL_LOSS Partial loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_COMPLETE_LOSS Complete loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_INVALID_SIZE One of the arguments sizes
 *      is invalid (bigger than the size of a long).
 */
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

/**
 * @brief Copies one mask to another.
 *
 * @param src The source for the copy operation.
 * @param src_size The size of @a src.
 * @param[out] dst The destination for the copy operation.
 *
 * @retval PROCESSOR_MASK_COPY_LOSSLESS It is guaranteed that the copy
 *      operation is lossless.
 * @retval PROCESSOR_MASK_COPY_PARTIAL_LOSS Partial loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_COMPLETE_LOSS Complete loss happened due
 *      to the sizes of @a src and @a dst.
 * @retval PROCESSOR_MASK_COPY_INVALID_SIZE One of the arguments sizes
 *      is invalid (bigger than the size of a long).
 */
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
