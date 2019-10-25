/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief Heap Handler Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAPIMPL_H
#define _RTEMS_SCORE_HEAPIMPL_H

#include <rtems/score/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreHeap
 *
 * @{
 */

/**
 * @brief See also @ref Heap_Block.size_and_flag.
 */
#define HEAP_PREV_BLOCK_USED ((uintptr_t) 1)

/**
 * @brief Size of the part at the block begin which may be used for allocation
 * in charge of the previous block.
 */
#define HEAP_ALLOC_BONUS sizeof(uintptr_t)

/**
 * @brief See _Heap_Resize_block().
 */
typedef enum {
  HEAP_RESIZE_SUCCESSFUL,
  HEAP_RESIZE_UNSATISFIED,
  HEAP_RESIZE_FATAL_ERROR
} Heap_Resize_status;

/**
 * @brief Gets the first and last block for the heap area.
 *
 * Nothing will be written to this area.
 *
 * @param heap_area_begin The starting address of the heap area.
 * @param heap_area_size The size of the heap area.
 * @param page_size The page size for the calculation.
 * @param min_block_size The minimal block size for the calculation.
 * @param[out] first_block_ptr The pointer to the first block in the case of success
 * @param[out] last_block_ptr The pointer to the last block in the case of success
 *
 * @retval true The area is big enough.
 * @retval false The area is not big enough.
 */
bool _Heap_Get_first_and_last_block(
  uintptr_t heap_area_begin,
  uintptr_t heap_area_size,
  uintptr_t page_size,
  uintptr_t min_block_size,
  Heap_Block **first_block_ptr,
  Heap_Block **last_block_ptr
);

/**
 * @brief Initializes the heap control block.
 *
 * Blocks of memory are allocated from the heap in multiples of @a page_size
 * byte units.  If the @a page_size is equal to zero or is not multiple of
 * @c CPU_ALIGNMENT, it is aligned up to the nearest @c CPU_ALIGNMENT boundary.
 *
 * @param[out] heap The heap control block to manage the area.
 * @param area_begin The starting address of the area.
 * @param area_size The size of the area in bytes.
 * @param page_size The page size for the calculation
 *
 * @retval some_value The maximum memory available.
 * @retval 0 The initialization failed.
 *
 * @see Heap_Initialization_or_extend_handler.
 */
uintptr_t _Heap_Initialize(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size
);

/**
 * @brief Allocates an aligned memory area with boundary constraint.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 * @param alignment The allocated memory area will begin at an address aligned by this value.
 * @param boundary The allocated memory area will fulfill a boundary constraint,
 *      if this value is not equal to zero.  The boundary value specifies
 *      the set of addresses which are aligned by the boundary value.  The
 *      interior of the allocated memory area will not contain an element of this
 *      set.  The begin or end address of the area may be a member of the set.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
void *_Heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief Allocates an aligned memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 * @param alignment The allocated memory area will begin at an address aligned by this value.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
RTEMS_INLINE_ROUTINE void *_Heap_Allocate_aligned(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment
)
{
  return _Heap_Allocate_aligned_with_boundary( heap, size, alignment, 0 );
}

/**
 * @brief Allocates a memory area.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * @param[in, out] heap The heap to allocate a memory are from.
 * @param size The size of the desired memory are in bytes.
 *
 * @retval pointer The starting address of the allocated memory area.
 * @retval NULL No memory is available of the parameters are inconsistent.
 */
RTEMS_INLINE_ROUTINE void *_Heap_Allocate( Heap_Control *heap, uintptr_t size )
{
  return _Heap_Allocate_aligned_with_boundary( heap, size, 0, 0 );
}

/**
 * @brief Frees the allocated memory area.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 *
 * @param[in, out] heap The heap of the allocated memory area.
 * @param addr The starting address of the memory area to be freed.
 *
 * @retval true The allocated memory area was successfully freed.
 * @retval false The method failed.
 */
bool _Heap_Free( Heap_Control *heap, void *addr );

/**
 * @brief Verifies the integrity of the heap.
 *
 * Walks the heap to verify its integrity.
 *
 * @param heap The heap whose integrity is to be verified.
 * @param source If @a dump is @c true, this is used to mark the output lines.
 * @param dump Indicates whether diagnostic messages will be printed to standard output.
 *
 * @retval true No errors occured, the heap´s integrity is not violated.
 * @retval false The heap is corrupt.
 */
bool _Heap_Walk(
  Heap_Control *heap,
  int source,
  bool dump
);

/**
 * @brief Heap block visitor.
 *
 * @see _Heap_Iterate().
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 */
typedef bool (*Heap_Block_visitor)(
  const Heap_Block *block,
  uintptr_t block_size,
  bool block_is_used,
  void *visitor_arg
);

/**
 * @brief Iterates over all blocks of the heap.
 *
 * @param[in, out] heap The heap to iterate over.
 * @param visitor This will be called for each heap block with
 *      the argument @a visitor_arg.
 * @param[in, out] visitor_arg The argument for all calls of @a visitor.
 */
void _Heap_Iterate(
  Heap_Control *heap,
  Heap_Block_visitor visitor,
  void *visitor_arg
);

/**
 * @brief Greedily allocates and empties the heap.
 *
 * Afterwards, the heap has at most @a block_count allocatable blocks of sizes
 * specified by @a block_sizes.  All other blocks are used.
 *
 * @param[in, out] heap The heap to operate upon
 * @param block_sizes The sizes of the allocatable blocks.  Must point to an
 *      array with @a block_count members.
 * @param block_count The maximum number of allocatable blocks of sizes
 *      specified by @block_sizes.
 *
 * @return Pointer to the first allocated block.
 *
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate(
  Heap_Control *heap,
  const uintptr_t *block_sizes,
  size_t block_count
);

/**
 * @brief Greedily allocates all blocks except the largest free block.
 *
 * Afterwards the heap has at most one allocatable block.  This block is the
 * largest free block if it exists.  All other blocks are used.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param[out] allocatable_size Stores the size of the largest free block of
 *      the heap after the method call.
 *
 * @return Pointer to the first allocated block.
 *
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate_all_except_largest(
  Heap_Control *heap,
  uintptr_t *allocatable_size
);

/**
 * @brief Frees blocks of a greedy allocation.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param blocks Must be the return value of _Heap_Greedy_allocate().
 */
void _Heap_Greedy_free(
  Heap_Control *heap,
  Heap_Block *blocks
);

/**
 * @brief Returns information about used and free blocks for the heap.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information of the @a heap after the method call.
 */
void _Heap_Get_information(
  Heap_Control *heap,
  Heap_Information_block *info
);

/**
 * @brief Returns information about free blocks for the heap.
 *
 * @param heap The heap to get the information from.
 * @param[out] info Stores the information about free blocks of @a heap after the
 *      method call.
 */
void _Heap_Get_free_information(
  Heap_Control *heap,
  Heap_Information *info
);

/**
 * @brief Returns the size of the allocatable memory area.
 *
 * The size value may be greater than the initially requested size in
 * _Heap_Allocate_aligned_with_boundary().
 *
 * Inappropriate values for @a addr will not corrupt the heap, but may yield
 * invalid size values.
 *
 * @param heap The heap to operate upon.
 * @param addr The starting address of the allocatable memory area.
 * @param[out] size Stores the size of the allocatable memory area after the method call.
 *
 * @retval true The operation was successful.
 * @retval false The operation was not successful.
 */
bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *addr,
  uintptr_t *size
);

/**
 * @brief Resizes the block of the allocated memory area.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param addr The starting address of the allocated memory area to be resized.
 * @param size The least possible size for the new memory area.  Resize may be
 *      impossible and depends on the current heap usage.
 * @param[out] old_size Stores the size available for allocation in the current
 *      block before the resize after the method call.
 * @param[out] new_size Stores the size available for allocation in the resized
 *      block after the method call.  In the case of an unsuccessful resize,
 *      zero is returned in this parameter
 *
 * @retval HEAP_RESIZE_SUCCESSFUL The resize was successful.
 * @retval HEAP_RESIZE_UNSATISFIED The least possible size @a size was too big.
 *      Resize not possible.
 * @retval HEAP_RESIZE_FATAL_ERROR The block starting at @a addr is not part of
 *      the heap.
 */
Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void *addr,
  uintptr_t size,
  uintptr_t *old_size,
  uintptr_t *new_size
);

/**
 * @brief Allocates the memory area.
 * starting at @a alloc_begin of size
 * @a alloc_size bytes in the block @a block.
 *
 * The block may be split up into multiple blocks.  The previous and next block
 * may be used or free.  Free block parts which form a vaild new block will be
 * inserted into the free list or merged with an adjacent free block.  If the
 * block is used, they will be inserted after the free list head.  If the block
 * is free, they will be inserted after the previous block in the free list.
 *
 * Inappropriate values for @a alloc_begin or @a alloc_size may corrupt the
 * heap.
 *
 * @param[in, out] heap The heap to operate upon.
 * @param block The block in which the memory area should be allocated
 * @param alloc_begin The starting address of the memory area that shall be allocated.
 * @param alloc_size The size of the desired allocated area in bytes.
 *
 * @return The block containing the allocated memory area.
 */
Heap_Block *_Heap_Block_allocate(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_begin,
  uintptr_t alloc_size
);

#ifndef HEAP_PROTECTION
  #define _Heap_Protection_block_initialize( heap, block ) ((void) 0)
  #define _Heap_Protection_block_check( heap, block ) ((void) 0)
  #define _Heap_Protection_block_error( heap, block, reason ) ((void) 0)
  #define _Heap_Protection_free_all_delayed_blocks( heap ) ((void) 0)
#else
  static inline void _Heap_Protection_block_initialize(
    Heap_Control *heap,
    Heap_Block *block
  )
  {
    (*heap->Protection.block_initialize)( heap, block );
  }

  static inline void _Heap_Protection_block_check(
    Heap_Control *heap,
    Heap_Block *block
  )
  {
    (*heap->Protection.block_check)( heap, block );
  }

  static inline void _Heap_Protection_block_error(
    Heap_Control *heap,
    Heap_Block *block,
    Heap_Error_reason reason
  )
  {
    (*heap->Protection.block_error)( heap, block, reason );
  }

  static inline void _Heap_Protection_free_all_delayed_blocks( Heap_Control *heap )
  {
    uintptr_t large = 0
      - (uintptr_t) HEAP_BLOCK_HEADER_SIZE
      - (uintptr_t) HEAP_ALLOC_BONUS
      - (uintptr_t) 1;
    void *p = _Heap_Allocate( heap, large );
    _Heap_Free( heap, p );
  }
#endif

/**
 * @brief Sets the fraction of delayed free blocks that is actually freed
 * during memory shortage.
 *
 * The default is to free half the delayed free blocks.  This is equal to a
 * fraction value of two.
 *
 * @param[in, out] heap The heap control.
 * @param fraction The fraction is one divided by this fraction value.
 */
RTEMS_INLINE_ROUTINE void _Heap_Protection_set_delayed_free_fraction(
  Heap_Control *heap,
  uintptr_t fraction
)
{
#ifdef HEAP_PROTECTION
  heap->Protection.delayed_free_fraction = fraction;
#else
  (void) heap;
  (void) fraction;
#endif
}

/**
 * @brief Returns the head of the free list of the heap.
 *
 * @param heap The heap to operate upon.
 *
 * @return The head of the free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_head( Heap_Control *heap )
{
  return &heap->free_list;
}

/**
 * @brief Returns the tail of the free list of the heap.
 *
 * @param heap The heap to operate upon.
 *
 * @return The tail of the free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_tail( Heap_Control *heap )
{
  return &heap->free_list;
}

/**
 * @brief Returns the first block of the free list of the heap.
 *
 * @param heap The heap to operate upon.
 *
 * @return The first block of the free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_first( Heap_Control *heap )
{
  return _Heap_Free_list_head(heap)->next;
}

/**
 * @brief Returns the last block of the free list of the heap.
 *
 * @param heap The heap to operate upon.
 *
 * @return The last block of the free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_last( Heap_Control *heap )
{
  return _Heap_Free_list_tail(heap)->prev;
}

/**
 * @brief Removes the block from the free list.
 *
 * @param block The block to be removed.
 */
RTEMS_INLINE_ROUTINE void _Heap_Free_list_remove( Heap_Block *block )
{
  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;

  prev->next = next;
  next->prev = prev;
}

/**
 * @brief Replaces one block in the free list by another.
 *
 * @param old_block The block in the free list to replace.
 * @param new_block The block that should replace @a old_block.
 */
RTEMS_INLINE_ROUTINE void _Heap_Free_list_replace(
  Heap_Block *old_block,
  Heap_Block *new_block
)
{
  Heap_Block *next = old_block->next;
  Heap_Block *prev = old_block->prev;

  new_block->next = next;
  new_block->prev = prev;

  next->prev = new_block;
  prev->next = new_block;
}

/**
 * @brief Inserts a block after an existing block in the free list.
 *
 * @param block_before The block that is already in the free list.
 * @param new_block The block to be inserted after @a block_before.
 */
RTEMS_INLINE_ROUTINE void _Heap_Free_list_insert_after(
  Heap_Block *block_before,
  Heap_Block *new_block
)
{
  Heap_Block *next = block_before->next;

  new_block->next = next;
  new_block->prev = block_before;
  block_before->next = new_block;
  next->prev = new_block;
}

/**
 * @brief Inserts a block before an existing block in the free list.
 *
 * @param block_before The block that is already in the free list.
 * @param new_block The block to be inserted before @a block_before.
 */
RTEMS_INLINE_ROUTINE void _Heap_Free_list_insert_before(
  Heap_Block *block_next,
  Heap_Block *new_block
)
{
  Heap_Block *prev = block_next->prev;

  new_block->next = block_next;
  new_block->prev = prev;
  prev->next = new_block;
  block_next->prev = new_block;
}

/**
 * @brief Checks if the value is aligned to the given alignment.
 *
 * @param value The value to check the alignment of.
 * @param alignment The alignment for the operation.
 *
 * @retval true The value is aligned to the given alignment.
 * @retval false The value is not aligned to the given alignment.
 */
RTEMS_INLINE_ROUTINE bool _Heap_Is_aligned(
  uintptr_t value,
  uintptr_t alignment
)
{
  return (value % alignment) == 0;
}

/**
 * @brief Returns the aligned value, truncating.
 *
 * @param value The value to be aligned
 * @param alignment The alignment for the operation.
 *
 * @return The aligned value, truncated.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Align_down(
  uintptr_t value,
  uintptr_t alignment
)
{
  return value - (value % alignment);
}

/**
 * @brief Returns the block which is @a offset away from @a block.
 *
 * @param block The block for the relative calculation.
 * @param offset The offset for the calculation.
 *
 * @return The address of the block which is @a offset away from @a block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  const Heap_Block *block,
  uintptr_t offset
)
{
  return (Heap_Block *) ((uintptr_t) block + offset);
}

/**
 * @brief Returns the address of the previous block.
 *
 * @param block The block of which the address of the previous block is requested.
 *
 * @return The address of the previous block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Prev_block(
  const Heap_Block *block
)
{
  return (Heap_Block *) ((uintptr_t) block - block->prev_size);
}

/**
 * @brief Returns the first address in the block without the heap header.
 *
 * @param block The block for the operation.
 *
 * @return The first address after the heap header.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Alloc_area_of_block(
  const Heap_Block *block
)
{
  return (uintptr_t) block + HEAP_BLOCK_HEADER_SIZE;
}

/**
 * @brief Returns the starting address of the block corresponding to the allocatable area.
 *
 * @param alloc_begin The starting address of the allocatable area.
 * @param page_size The page size for the calculation.
 *
 * @return The Starting address of the corresponding block of the allocatable area.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_of_alloc_area(
  uintptr_t alloc_begin,
  uintptr_t page_size
)
{
  return (Heap_Block *) (_Heap_Align_down( alloc_begin, page_size )
    - HEAP_BLOCK_HEADER_SIZE);
}

/**
 * @brief Returns the block size.
 *
 * @param block The block of which the size is requested.
 *
 * @return The block size.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Block_size( const Heap_Block *block )
{
  return block->size_and_flag & ~HEAP_PREV_BLOCK_USED;
}

/**
 * @brief Sets the block size.
 *
 * @param[in, out] block The block of which the size shall be set.
 * @param size The new size of the block.
 */
RTEMS_INLINE_ROUTINE void _Heap_Block_set_size(
  Heap_Block *block,
  uintptr_t size
)
{
  uintptr_t flag = block->size_and_flag & HEAP_PREV_BLOCK_USED;

  block->size_and_flag = size | flag;
}

/**
 * @brief Returns if the previous heap block is used.
 *
 * @param block The block of which the information about the previous
 *      block is requested.
 *
 * @retval true The previous block is used.
 * @retval false The previous block is not used.
 */
RTEMS_INLINE_ROUTINE bool _Heap_Is_prev_used( const Heap_Block *block )
{
  return block->size_and_flag & HEAP_PREV_BLOCK_USED;
}

/**
 * @brief Returns if the heap block is used.
 *
 * @param block The block of which the information is requested.
 *
 * @retval true The block is used.
 * @retval false The block is not used.
 */
RTEMS_INLINE_ROUTINE bool _Heap_Is_used(
  const Heap_Block *block
)
{
  const Heap_Block *const next_block =
    _Heap_Block_at( block, _Heap_Block_size( block ) );

  return _Heap_Is_prev_used( next_block );
}

/**
 * @brief Returns if the heap block is free.
 *
 * @param block The block of which the information is requested.
 *
 * @retval true The block is free.
 * @retval false The block is not free.
 */
RTEMS_INLINE_ROUTINE bool _Heap_Is_free(
  const Heap_Block *block
)
{
  return !_Heap_Is_used( block );
}

/**
 * @brief Returns if the block is part of the heap.
 *
 * @param heap The heap to test if the @a block is part of it.
 * @param block The block of which the information is requested.
 *
 * @retval true The block is part of the heap.
 * @retval false The block is not part of the heap.
 */
RTEMS_INLINE_ROUTINE bool _Heap_Is_block_in_heap(
  const Heap_Control *heap,
  const Heap_Block *block
)
{
  return (uintptr_t) block >= (uintptr_t) heap->first_block
    && (uintptr_t) block <= (uintptr_t) heap->last_block;
}

/**
 * @brief Sets the size of the last block for the heap.
 *
 * The next block of the last block will be the first block.  Since the first
 * block indicates that the previous block is used, this ensures that the last
 * block appears as used for the _Heap_Is_used() and _Heap_Is_free()
 * functions.
 *
 * This feature will be used to terminate the scattered heap area list.  See
 * also _Heap_Extend().
 *
 * @param[in, out] heap The heap to set the last block size of.
 */
RTEMS_INLINE_ROUTINE void _Heap_Set_last_block_size( Heap_Control *heap )
{
  _Heap_Block_set_size(
    heap->last_block,
    (uintptr_t) heap->first_block - (uintptr_t) heap->last_block
  );
}

/**
 * @brief Returns the size of the allocatable area in bytes.
 *
 * This value is an integral multiple of the page size.
 *
 * @param heap The heap to get the allocatable area from.
 *
 * @return The size of the allocatable area in @a heap in bytes.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Get_size( const Heap_Control *heap )
{
  return heap->stats.size;
}

/**
 * @brief Returns the bigger one of the two arguments.
 *
 * @param a The parameter on the left hand side of the comparison.
 * @param b The parameter on the right hand side of the comparison.
 *
 * @retval a If a > b.
 * @retval b If b >= a
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Max( uintptr_t a, uintptr_t b )
{
  return a > b ? a : b;
}

/**
 * @brief Returns the smaller one of the two arguments.
 *
 * @param a The parameter on the left hand side of the comparison.
 * @param b The parameter on the right hand side of the comparison.
 *
 * @retval a If a < b.
 * @retval b If b <= a
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Min( uintptr_t a, uintptr_t b )
{
  return a < b ? a : b;
}

#ifdef RTEMS_DEBUG
  #define RTEMS_HEAP_DEBUG
#endif

#ifdef RTEMS_HEAP_DEBUG
  #include <assert.h>
  #define _HAssert( cond ) \
    do { \
      if ( !(cond) ) { \
        __assert( __FILE__, __LINE__, #cond ); \
      } \
    } while (0)
#else
  #define _HAssert( cond ) ((void) 0)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
