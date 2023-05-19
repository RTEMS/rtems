/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProcessorMask
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreProcessorMask.
 */

/*
 * Copyright (C) 2016, 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_PROCESSORMASK_H
#define _RTEMS_SCORE_PROCESSORMASK_H

#include <rtems/score/cpu.h>

#include <sys/cpuset.h>

#include <strings.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Recent Newlib versions provide the bitset defines in the system reserved
 * namespace.
 */
#ifndef __BIT_AND2
#define __BIT_AND2 BIT_AND2
#endif
#ifndef __BIT_CLR
#define __BIT_CLR BIT_CLR
#endif
#ifndef __BIT_CMP
#define __BIT_CMP BIT_CMP
#endif
#ifndef __BIT_COPY
#define __BIT_COPY BIT_COPY
#endif
#ifndef __BIT_COUNT
#define __BIT_COUNT BIT_COUNT
#endif
#ifndef __BITSET_DEFINE
#define __BITSET_DEFINE BITSET_DEFINE
#endif
#ifndef __BIT_EMPTY
#define __BIT_EMPTY BIT_EMPTY
#endif
#ifndef __BIT_FILL
#define __BIT_FILL BIT_FILL
#endif
#ifndef __BIT_FLS
#define __BIT_FLS BIT_FLS
#endif
#ifndef __BIT_ISSET
#define __BIT_ISSET BIT_ISSET
#endif
#ifndef __BIT_OR2
#define __BIT_OR2 BIT_OR2
#endif
#ifndef __BIT_OVERLAP
#define __BIT_OVERLAP BIT_OVERLAP
#endif
#ifndef __BIT_SET
#define __BIT_SET BIT_SET
#endif
#ifndef __BIT_SETOF
#define __BIT_SETOF BIT_SETOF
#endif
#ifndef __BIT_SUBSET
#define __BIT_SUBSET BIT_SUBSET
#endif
#ifndef __BIT_XOR2
#define __BIT_XOR2 BIT_XOR2
#endif
#ifndef __BIT_ZERO
#define __BIT_ZERO BIT_ZERO
#endif

/**
 * @defgroup RTEMSScoreProcessorMask Processor Mask
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support processor masks.
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
typedef __BITSET_DEFINE( Processor_mask, CPU_MAXIMUM_PROCESSORS ) Processor_mask;

/**
 * @brief Sets the bits of the mask to zero, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] mask The mask to set to zero.
 */
static inline void _Processor_mask_Zero( Processor_mask *mask )
{
  __BIT_ZERO( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Checks if the mask is zero, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param mask The mask to check whether is is zero
 *
 * @retval true The mask is zero.
 * @retval false The mask is not zero.
 */
static inline bool _Processor_mask_Is_zero( const Processor_mask *mask )
{
  return __BIT_EMPTY( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Fills the mask, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] mask The mask to fill
 */
static inline void _Processor_mask_Fill( Processor_mask *mask )
{
  __BIT_FILL( CPU_MAXIMUM_PROCESSORS, mask );
}

/**
 * @brief Copies the mask to another mask, also considers CPU_MAXIMUM_PROCESSORS.
 *
 * @param[out] dst The mask to copy @a src to.
 * @param src The mask to copy to @a dst.
 */
static inline void _Processor_mask_Assign(
  Processor_mask *dst, const Processor_mask *src
)
{
  __BIT_COPY( CPU_MAXIMUM_PROCESSORS, src, dst );
}

/**
 * @brief Sets the specified index bit of the mask.
 *
 * @param[out] mask The mask to set the bit of.
 * @param index The index of the bit that shall be set.
 */
static inline void _Processor_mask_Set(
  Processor_mask *mask,
  uint32_t        index
)
{
  __BIT_SET( CPU_MAXIMUM_PROCESSORS, index, mask );
}

/**
 * @brief Clears the specified index bit of the mask.
 *
 * @param[out] mask The mask to clear the bit of.
 * @param index The index of the bit that shall be cleared.
 */
static inline void _Processor_mask_Clear(
  Processor_mask *mask,
  uint32_t        index
)
{
  __BIT_CLR( CPU_MAXIMUM_PROCESSORS, index, mask );
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
static inline bool _Processor_mask_Is_set(
  const Processor_mask *mask,
  uint32_t              index
)
{
  return __BIT_ISSET( CPU_MAXIMUM_PROCESSORS, index, mask );
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
static inline bool _Processor_mask_Is_equal(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return !__BIT_CMP( CPU_MAXIMUM_PROCESSORS, a, b );
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
static inline bool _Processor_mask_Has_overlap(
  const Processor_mask *a,
  const Processor_mask *b
)
{
  return __BIT_OVERLAP( CPU_MAXIMUM_PROCESSORS, a, b );
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
static inline bool _Processor_mask_Is_subset(
  const Processor_mask *big,
  const Processor_mask *small
)
{
  return __BIT_SUBSET( CPU_MAXIMUM_PROCESSORS, big, small );
}

/**
 * @brief Performs a bitwise a = b & c.
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the AND-operation.
 * @param c The second parameter of the AND-operation.
 */
static inline void _Processor_mask_And(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  __BIT_AND2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Performs a bitwise a = b | c.
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the OR-operation.
 * @param c The second parameter of the OR-operation.
 */
static inline void _Processor_mask_Or(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  __BIT_OR2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Performs a bitwise a = b ^ c.
 *
 * @param[out] a The processor mask that is set by this operation.
 * @param b The first parameter of the XOR-operation.
 * @param c The second parameter of the XOR-operation.
 */
static inline void _Processor_mask_Xor(
  Processor_mask       *a,
  const Processor_mask *b,
  const Processor_mask *c
)
{
  __BIT_XOR2( CPU_MAXIMUM_PROCESSORS, a, b, c );
}

/**
 * @brief Gets the number of set bits in the processor mask.
 *
 * @param a The processor mask of which the set bits are counted.
 *
 * @return The number of set bits in @a a.
 */
static inline uint32_t _Processor_mask_Count( const Processor_mask *a )
{
  return (uint32_t) __BIT_COUNT( CPU_MAXIMUM_PROCESSORS, a );
}

/**
 * @brief Finds the last set of the processor mask.
 *
 * @param a The processor mask wo find the last set of.
 *
 * @return The last set of @a a.
 */
static inline uint32_t _Processor_mask_Find_last_set( const Processor_mask *a )
{
  return (uint32_t) __BIT_FLS( CPU_MAXIMUM_PROCESSORS, a );
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
static inline uint32_t _Processor_mask_To_uint32_t(
  const Processor_mask *mask,
  uint32_t              index
)
{
  long bits = mask->__bits[ index / _BITSET_BITS ];

  return (uint32_t) ( bits >> ( 32 * ( ( index % _BITSET_BITS ) / 32 ) ) );
}

/**
 * @brief Creates a processor set from an unsigned 32-bit integer relative to
 * the specified index.
 *
 * @param[out] mask The mask that is created.
 * @param bits The bits for creating the mask.
 * @param index The index to which the mask is relative.
 */
static inline void _Processor_mask_From_uint32_t(
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
static inline void _Processor_mask_From_index(
  Processor_mask *mask,
  uint32_t        index
)
{
  __BIT_SETOF( CPU_MAXIMUM_PROCESSORS, (int) index, mask );
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
static inline bool _Processor_mask_Is_at_most_partial_loss(
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
static inline Processor_mask_Copy_status _Processor_mask_To_cpu_set_t(
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
static inline Processor_mask_Copy_status _Processor_mask_From_cpu_set_t(
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
