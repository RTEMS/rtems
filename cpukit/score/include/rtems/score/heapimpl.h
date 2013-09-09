/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAPIMPL_H
#define _RTEMS_SCORE_HEAPIMPL_H

#include <rtems/score/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreHeap
 */
/**@{**/

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
 * @brief Gets the first and last block for the heap area with begin
 * @a heap_area_begin and size @a heap_area_size.
 *
 * A page size of @a page_size and minimal block size of @a min_block_size will
 * be used for calculation.
 *
 * Nothing will be written to this area.
 *
 * In case of success the pointers to the first and last block will be returned
 * via @a first_block_ptr and @a last_block_ptr.
 *
 * Returns @c true if the area is big enough, and @c false otherwise.
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
 * @brief Initializes the heap control block @a heap to manage the area
 * starting at @a area_begin of size @a area_size bytes.
 *
 * Blocks of memory are allocated from the heap in multiples of @a page_size
 * byte units.  If the @a page_size is equal to zero or is not multiple of
 * @c CPU_ALIGNMENT, it is aligned up to the nearest @c CPU_ALIGNMENT boundary.
 *
 * Returns the maximum memory available, or zero in case of failure.
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
 * @brief Allocates a memory area of size @a size bytes from the heap @a heap.
 *
 * If the alignment parameter @a alignment is not equal to zero, the allocated
 * memory area will begin at an address aligned by this value.
 *
 * If the boundary parameter @a boundary is not equal to zero, the allocated
 * memory area will fulfill a boundary constraint.  The boundary value
 * specifies the set of addresses which are aligned by the boundary value.  The
 * interior of the allocated memory area will not contain an element of this
 * set.  The begin or end address of the area may be a member of the set.
 *
 * A size value of zero will return a unique address which may be freed with
 * _Heap_Free().
 *
 * Returns a pointer to the begin of the allocated memory area, or @c NULL if
 * no memory is available or the parameters are inconsistent.
 */
void *_Heap_Allocate_aligned_with_boundary(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief See _Heap_Allocate_aligned_with_boundary() with boundary equals zero.
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
 * @brief See _Heap_Allocate_aligned_with_boundary() with alignment and
 * boundary equals zero.
 */
RTEMS_INLINE_ROUTINE void *_Heap_Allocate( Heap_Control *heap, uintptr_t size )
{
  return _Heap_Allocate_aligned_with_boundary( heap, size, 0, 0 );
}

/**
 * @brief Frees the allocated memory area starting at @a addr in the heap
 * @a heap.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 *
 * Returns @c true in case of success, and @c false otherwise.
 */
bool _Heap_Free( Heap_Control *heap, void *addr );

/**
 * @brief Walks the heap @a heap to verify its integrity.
 *
 * If @a dump is @c true, then diagnostic messages will be printed to standard
 * output.  In this case @a source is used to mark the output lines.
 *
 * Returns @c true if no errors occurred, and @c false if the heap is corrupt.
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
 * For each block the @a visitor with the argument @a visitor_arg will be
 * called.
 */
void _Heap_Iterate(
  Heap_Control *heap,
  Heap_Block_visitor visitor,
  void *visitor_arg
);

/**
 * @brief Greedy allocate that empties the heap.
 *
 * Afterwards the heap has at most @a block_count allocatable blocks of sizes
 * specified by @a block_sizes.  The @a block_sizes must point to an array with
 * @a block_count members.  All other blocks are used.
 *
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate(
  Heap_Control *heap,
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
 * @see _Heap_Greedy_free().
 */
Heap_Block *_Heap_Greedy_allocate_all_except_largest(
  Heap_Control *heap,
  uintptr_t *allocatable_size
);

/**
 * @brief Frees blocks of a greedy allocation.
 *
 * The @a blocks must be the return value of _Heap_Greedy_allocate().
 */
void _Heap_Greedy_free(
  Heap_Control *heap,
  Heap_Block *blocks
);

/**
 * @brief Returns information about used and free blocks for the heap @a heap
 * in @a info.
 */
void _Heap_Get_information(
  Heap_Control *heap,
  Heap_Information_block *info
);

/**
 * @brief Returns information about free blocks for the heap @a heap in
 * @a info.
 */
void _Heap_Get_free_information(
  Heap_Control *heap,
  Heap_Information *info
);

/**
 * @brief Returns the size of the allocatable memory area starting at @a addr
 * in @a size.
 *
 * The size value may be greater than the initially requested size in
 * _Heap_Allocate_aligned_with_boundary().
 *
 * Inappropriate values for @a addr will not corrupt the heap, but may yield
 * invalid size values.
 *
 * Returns @a true if successful, and @c false otherwise.
 */
bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *addr,
  uintptr_t *size
);

/**
 * @brief Resizes the block of the allocated memory area starting at @a addr.
 *
 * The new memory area will have a size of at least @a size bytes.  A resize
 * may be impossible and depends on the current heap usage.
 *
 * The size available for allocation in the current block before the resize
 * will be returned in @a old_size.  The size available for allocation in
 * the resized block will be returned in @a new_size.  If the resize was not
 * successful, then a value of zero will be returned in @a new_size.
 *
 * Inappropriate values for @a addr may corrupt the heap.
 */
Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void *addr,
  uintptr_t size,
  uintptr_t *old_size,
  uintptr_t *new_size
);

/**
 * @brief Allocates the memory area starting at @a alloc_begin of size
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
 * Returns the block containing the allocated memory area.
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
  #define _Heap_Protection_block_error( heap, block ) ((void) 0)
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
    Heap_Block *block
  )
  {
    (*heap->Protection.block_error)( heap, block );
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

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_head( Heap_Control *heap )
{
  return &heap->free_list;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_tail( Heap_Control *heap )
{
  return &heap->free_list;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_first( Heap_Control *heap )
{
  return _Heap_Free_list_head(heap)->next;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Free_list_last( Heap_Control *heap )
{
  return _Heap_Free_list_tail(heap)->prev;
}

RTEMS_INLINE_ROUTINE void _Heap_Free_list_remove( Heap_Block *block )
{
  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;

  prev->next = next;
  next->prev = prev;
}

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

RTEMS_INLINE_ROUTINE bool _Heap_Is_aligned(
  uintptr_t value,
  uintptr_t alignment
)
{
  return (value % alignment) == 0;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Align_down(
  uintptr_t value,
  uintptr_t alignment
)
{
  return value - (value % alignment);
}

/**
 * @brief Returns the block which is @a offset away from @a block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  const Heap_Block *block,
  uintptr_t offset
)
{
  return (Heap_Block *) ((uintptr_t) block + offset);
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Prev_block(
  const Heap_Block *block
)
{
  return (Heap_Block *) ((uintptr_t) block - block->prev_size);
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Alloc_area_of_block(
  const Heap_Block *block
)
{
  return (uintptr_t) block + HEAP_BLOCK_HEADER_SIZE;
}

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_of_alloc_area(
  uintptr_t alloc_begin,
  uintptr_t page_size
)
{
  return (Heap_Block *) (_Heap_Align_down( alloc_begin, page_size )
    - HEAP_BLOCK_HEADER_SIZE);
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Block_size( const Heap_Block *block )
{
  return block->size_and_flag & ~HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE void _Heap_Block_set_size(
  Heap_Block *block,
  uintptr_t size
)
{
  uintptr_t flag = block->size_and_flag & HEAP_PREV_BLOCK_USED;

  block->size_and_flag = size | flag;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_prev_used( const Heap_Block *block )
{
  return block->size_and_flag & HEAP_PREV_BLOCK_USED;
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_used(
  const Heap_Block *block
)
{
  const Heap_Block *const next_block =
    _Heap_Block_at( block, _Heap_Block_size( block ) );

  return _Heap_Is_prev_used( next_block );
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_free(
  const Heap_Block *block
)
{
  return !_Heap_Is_used( block );
}

RTEMS_INLINE_ROUTINE bool _Heap_Is_block_in_heap(
  const Heap_Control *heap,
  const Heap_Block *block
)
{
  return (uintptr_t) block >= (uintptr_t) heap->first_block
    && (uintptr_t) block <= (uintptr_t) heap->last_block;
}

/**
 * @brief Sets the size of the last block for heap @a heap.
 *
 * The next block of the last block will be the first block.  Since the first
 * block indicates that the previous block is used, this ensures that the last
 * block appears as used for the _Heap_Is_used() and _Heap_Is_free()
 * functions.
 *
 * This feature will be used to terminate the scattered heap area list.  See
 * also _Heap_Extend().
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
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Get_size( const Heap_Control *heap )
{
  return heap->stats.size;
}

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Max( uintptr_t a, uintptr_t b )
{
  return a > b ? a : b;
}

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
