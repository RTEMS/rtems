/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup MallocSupport
 *
 * @ingroup RTEMSAPIMalloc
 *
 * @brief This header file defines interfaces to support and use dynamic memory
 *   allocation.
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_MALLOC_H
#define _RTEMS_MALLOC_H

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/libcsupport.h> /* for malloc_walk() */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MallocSupport Malloc Support
 *
 * @ingroup libcsupport
 *
 * @brief This group contains interfaces to support dynamic memory allocation.
 *
 * @{
 */

/**
 *  @brief C program heap control.
 *
 *  This is the pointer to the heap control structure used to manage the C
 *  program heap.
 */
extern Heap_Control *RTEMS_Malloc_Heap;

void _Malloc_Initialize( void );

typedef void *(*rtems_heap_extend_handler)(
  Heap_Control *heap,
  size_t alloc_size
);

/**
 *  @brief RTEMS Extend Heap via Sbrk
 */
void *rtems_heap_extend_via_sbrk(
  Heap_Control *heap,
  size_t alloc_size
);

void *rtems_heap_null_extend(
  Heap_Control *heap,
  size_t alloc_size
);

extern const rtems_heap_extend_handler rtems_malloc_extend_handler;

/*
 * Malloc Plugin to Dirty Memory at Allocation Time
 */
typedef void (*rtems_malloc_dirtier_t)(void *, size_t);
extern rtems_malloc_dirtier_t rtems_malloc_dirty_helper;

/** @} */

/**
 * @defgroup RTEMSAPIMalloc Dynamic Memory Allocation
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains non-standard interfaces to use dynamic memory
 *   allocation.
 *
 * @{
 */

void rtems_heap_set_sbrk_amount( ptrdiff_t sbrk_amount );

/**
 * @brief Greedy allocate that empties the sbrk memory
 *
 * Afterwards all the sbrk avialable memory will have been allocated
 * to the provided heap.
 *
 * @see rtems_heap_extend_via_sbrk().
 */
void rtems_heap_sbrk_greedy_allocate(
  Heap_Control *heap,
  size_t alloc_size
);

/**
 *  @brief Dirty Memory Function
 *
 *  This method fills the specified area with a non-zero pattern
 *  to aid in debugging programs which do not initialize their
 *  memory allocated from the heap.
 */
void rtems_malloc_dirty_memory(
  void   *start,
  size_t  size
);

/**
 *  @brief RTEMS Variation on Aligned Memory Allocation
 *
 *  This method is a help memalign implementation which does all
 *  error checking done by posix_memalign() EXCEPT it does NOT
 *  place numeric restrictions on the alignment value.
 *
 *  @param[in] pointer points to the user pointer
 *  @param[in] alignment is the desired alignment
 *  @param[in] size is the allocation request size in bytes
 *
 *  @return This methods returns zero on success and a POSIX errno
 *          value to indicate the failure condition.  On success
 *          *pointer will contain the address of the allocated memory.
 */
int rtems_memalign(
  void   **pointer,
  size_t   alignment,
  size_t   size
);

/**
 * @brief Allocates a memory area of size @a size bytes from the heap.
 *
 * If the alignment parameter @a alignment is not equal to zero, the allocated
 * memory area will begin at an address aligned by this value.
 *
 * If the boundary parameter @a boundary is not equal to zero, the allocated
 * memory area will comply with a boundary constraint.  The boundary value
 * specifies the set of addresses which are aligned by the boundary value.  The
 * interior of the allocated memory area will not contain an element of this
 * set.  The begin or end address of the area may be a member of the set.
 *
 * A size value of zero will return a unique address which may be freed with
 * free().
 *
 * The memory allocated by this function can be released with a call to free().
 *
 * @return A pointer to the begin of the allocated memory area, or @c NULL if
 * no memory is available or the parameters are inconsistent.
 */
void *rtems_heap_allocate_aligned_with_boundary(
  size_t size,
  uintptr_t alignment,
  uintptr_t boundary
) RTEMS_MALLOCLIKE RTEMS_ALLOC_SIZE(1) RTEMS_ALLOC_ALIGN(2)
  RTEMS_WARN_UNUSED_RESULT;

/**
 * @brief Allocates a memory area of the specified size from the heap.
 *
 * This function is almost identical to malloc(). The only exception is that
 * errno is not set in case of a memory allocation failure.
 *
 * @param[in] size The memory area size in bytes.
 *
 * @retval NULL The memory allocation failed or @a size is zero.
 * @retval otherwise The begin address of the allocated memory area.
 */
void *rtems_malloc(size_t size)
  RTEMS_MALLOCLIKE RTEMS_ALLOC_SIZE(1) RTEMS_WARN_UNUSED_RESULT;

/**
 * @brief Allocates a memory area for the specified count of elements from the
 * heap.
 *
 * The allocated memory area is fully filled with zero bits.
 *
 * This function is almost identical to calloc(). The only exception is that
 * errno is not set in case of a memory allocation failure.
 *
 * @param[in] nelem The count of elements.
 * @param[in] elsize The size of each elements.
 *
 * @retval NULL The memory allocation failed or @a nelem is zero or @a elsize
 *   is zero.
 * @retval otherwise The begin address of the allocated memory area.
 */
void *rtems_calloc(size_t nelem, size_t elsize)
  RTEMS_MALLOCLIKE RTEMS_ALLOC_SIZE_2(1, 2) RTEMS_WARN_UNUSED_RESULT;

/**
 * @brief Extends the memory available for the heap using the memory area
 * starting at @a area_begin of size @a area_size bytes.
 *
 * There are no alignment requirements.  The memory area must be big enough to
 * contain some maintenance blocks.  It must not overlap parts of the current
 * heap areas.  Disconnected subordinate heap areas will lead to used blocks
 * which cover the gaps.  Extending with an inappropriate memory area will
 * corrupt the heap.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS Invalid memory area.
 */
rtems_status_code rtems_heap_extend(
  void *area_begin,
  uintptr_t area_size
);

/**
 * @brief Greedy allocate that empties the heap.
 *
 * Afterwards the heap has at most @a block_count allocatable blocks of sizes
 * specified by @a block_sizes.  The @a block_sizes must point to an array with
 * @a block_count members.  All other blocks are used.
 *
 * @see rtems_heap_greedy_free().
 */
void *rtems_heap_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
);

/**
 * @brief Greedy allocate all blocks except the largest free block.
 *
 * Afterwards the heap has at most one allocatable block.  This block is the
 * largest free block if it exists.  The allocatable size of this block is
 * stored in @a allocatable_size.  All other blocks are used.
 *
 * @see rtems_heap_greedy_free().
 */
void *rtems_heap_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
);

/**
 * @brief Frees space of a greedy allocation.
 *
 * The @a opaque argument must be the return value of
 * rtems_heap_greedy_allocate() or
 * rtems_heap_greedy_allocate_all_except_largest().
 */
void rtems_heap_greedy_free( void *opaque );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
