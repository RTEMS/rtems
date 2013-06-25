/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAP_H
#define _RTEMS_SCORE_HEAP_H

#include <rtems/system.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RTEMS_DEBUG
  #define HEAP_PROTECTION
#endif

/**
 * @defgroup ScoreHeap Heap Handler
 *
 * @ingroup Score
 *
 * @brief The Heap Handler provides a heap.
 *
 * A heap is a doubly linked list of variable size blocks which are allocated
 * using the first fit method.  Garbage collection is performed each time a
 * block is returned to the heap by coalescing neighbor blocks.  Control
 * information for both allocated and free blocks is contained in the heap
 * area.  A heap control structure contains control information for the heap.
 *
 * The alignment routines could be made faster should we require only powers of
 * two to be supported for page size, alignment and boundary arguments.  The
 * minimum alignment requirement for pages is currently CPU_ALIGNMENT and this
 * value is only required to be multiple of two and explicitly not required to
 * be a power of two.
 *
 * There are two kinds of blocks.  One sort describes a free block from which
 * we can allocate memory.  The other blocks are used and provide an allocated
 * memory area.  The free blocks are accessible via a list of free blocks.
 *
 * Blocks or areas cover a continuous set of memory addresses. They have a
 * begin and end address.  The end address is not part of the set.  The size of
 * a block or area equals the distance between the begin and end address in
 * units of bytes.
 *
 * Free blocks look like:
 * <table>
 *   <tr>
 *     <td rowspan=4>@ref Heap_Block</td><td>previous block size in case the
 *       previous block is free, <br> otherwise it may contain data used by
 *       the previous block</td>
 *   </tr>
 *   <tr>
 *     <td>block size and a flag which indicates if the previous block is free
 *       or used, <br> this field contains always valid data regardless of the
 *       block usage</td>
 *   </tr>
 *   <tr><td>pointer to next block (this field is page size aligned)</td></tr>
 *   <tr><td>pointer to previous block</td></tr>
 *   <tr><td colspan=2>free space</td></tr>
 * </table>
 *
 * Used blocks look like:
 * <table>
 *   <tr>
 *     <td rowspan=4>@ref Heap_Block</td><td>previous block size in case the
 *       previous block is free,<br>otherwise it may contain data used by
 *       the previous block</td>
 *   </tr>
 *   <tr>
 *     <td>block size and a flag which indicates if the previous block is free
 *       or used, <br> this field contains always valid data regardless of the
 *       block usage</td>
 *   </tr>
 *   <tr><td>begin of allocated area (this field is page size aligned)</td></tr>
 *   <tr><td>allocated space</td></tr>
 *   <tr><td colspan=2>allocated space</td></tr>
 * </table>
 *
 * The heap area after initialization contains two blocks and looks like:
 * <table>
 *   <tr><th>Label</th><th colspan=2>Content</th></tr>
 *   <tr><td>heap->area_begin</td><td colspan=2>heap area begin address</td></tr>
 *   <tr>
 *     <td>first_block->prev_size</td>
 *     <td colspan=2>
 *       subordinate heap area end address (this will be used to maintain a
 *       linked list of scattered heap areas)
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>first_block->size</td>
 *     <td colspan=2>size available for allocation
 *       | @c HEAP_PREV_BLOCK_USED</td>
 *   </tr>
 *   <tr>
 *     <td>first_block->next</td><td>_Heap_Free_list_tail(heap)</td>
 *     <td rowspan=3>memory area available for allocation</td>
 *   </tr>
 *   <tr><td>first_block->prev</td><td>_Heap_Free_list_head(heap)</td></tr>
 *   <tr><td>...</td></tr>
 *   <tr>
 *     <td>last_block->prev_size</td><td colspan=2>size of first block</td>
 *   </tr>
 *   <tr>
 *     <td>last_block->size</td>
 *     <td colspan=2>first block begin address - last block begin address</td>
 *   </tr>
 *   <tr><td>heap->area_end</td><td colspan=2>heap area end address</td></tr>
 * </table>
 * The next block of the last block is the first block.  Since the first
 * block indicates that the previous block is used, this ensures that the
 * last block appears as used for the _Heap_Is_used() and _Heap_Is_free()
 * functions.
 */
/**@{**/

typedef struct Heap_Control Heap_Control;

typedef struct Heap_Block Heap_Block;

#ifndef HEAP_PROTECTION
  #define HEAP_PROTECTION_HEADER_SIZE 0
#else
  #define HEAP_PROTECTOR_COUNT 2

  #define HEAP_BEGIN_PROTECTOR_0 ((uintptr_t) 0xfd75a98f)
  #define HEAP_BEGIN_PROTECTOR_1 ((uintptr_t) 0xbfa1f177)
  #define HEAP_END_PROTECTOR_0 ((uintptr_t) 0xd6b8855e)
  #define HEAP_END_PROTECTOR_1 ((uintptr_t) 0x13a44a5b)

  #define HEAP_FREE_PATTERN ((uintptr_t) 0xe7093cdf)

  #define HEAP_PROTECTION_OBOLUS ((Heap_Block *) 1)

  typedef void (*_Heap_Protection_handler)(
     Heap_Control *heap,
     Heap_Block *block
  );

  typedef struct {
    _Heap_Protection_handler block_initialize;
    _Heap_Protection_handler block_check;
    _Heap_Protection_handler block_error;
    void *handler_data;
    Heap_Block *first_delayed_free_block;
    Heap_Block *last_delayed_free_block;
    uintptr_t delayed_free_block_count;
  } Heap_Protection;

  typedef struct {
    uintptr_t protector [HEAP_PROTECTOR_COUNT];
    Heap_Block *next_delayed_free_block;
    Thread_Control *task;
    void *tag;
  } Heap_Protection_block_begin;

  typedef struct {
    uintptr_t protector [HEAP_PROTECTOR_COUNT];
  } Heap_Protection_block_end;

  #define HEAP_PROTECTION_HEADER_SIZE \
    (sizeof(Heap_Protection_block_begin) + sizeof(Heap_Protection_block_end))
#endif

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
 * @brief The block header consists of the two size fields
 * (@ref Heap_Block.prev_size and @ref Heap_Block.size_and_flag).
 */
#define HEAP_BLOCK_HEADER_SIZE \
  (2 * sizeof(uintptr_t) + HEAP_PROTECTION_HEADER_SIZE)

/**
 * @brief Description for free or used blocks.
 */
struct Heap_Block {
  /**
   * @brief Size of the previous block or part of the allocated area of the
   * previous block.
   *
   * This field is only valid if the previous block is free.  This case is
   * indicated by a cleared @c HEAP_PREV_BLOCK_USED flag in the
   * @a size_and_flag field of the current block.
   *
   * In a used block only the @a size_and_flag field needs to be valid.  The
   * @a prev_size field of the current block is maintained by the previous
   * block.  The current block can use the @a prev_size field in the next block
   * for allocation.
   */
  uintptr_t prev_size;

  #ifdef HEAP_PROTECTION
    Heap_Protection_block_begin Protection_begin;
  #endif

  /**
   * @brief Contains the size of the current block and a flag which indicates
   * if the previous block is free or used.
   *
   * If the flag @c HEAP_PREV_BLOCK_USED is set, then the previous block is
   * used, otherwise the previous block is free.  A used previous block may
   * claim the @a prev_size field for allocation.  This trick allows to
   * decrease the overhead in the used blocks by the size of the @a prev_size
   * field.  As sizes are required to be multiples of two, the least
   * significant bits would be always zero. We use this bit to store the flag.
   *
   * This field is always valid.
   */
  uintptr_t size_and_flag;

  #ifdef HEAP_PROTECTION
    Heap_Protection_block_end Protection_end;
  #endif

  /**
   * @brief Pointer to the next free block or part of the allocated area.
   *
   * This field is page size aligned and begins of the allocated area in case
   * the block is used.
   *
   * This field is only valid if the block is free and thus part of the free
   * block list.
   */
  Heap_Block *next;

  /**
   * @brief Pointer to the previous free block or part of the allocated area.
   *
   * This field is only valid if the block is free and thus part of the free
   * block list.
   */
  Heap_Block *prev;
};

/**
 * @brief Run-time heap statistics.
 *
 * The value @a searches / @a allocs gives the mean number of searches per
 * allocation, while @a max_search gives maximum number of searches ever
 * performed on a single allocation call.
 */
typedef struct {
  /**
   * @brief Instance number of this heap.
   */
  uint32_t instance;

  /**
   * @brief Size of the allocatable area in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t size;

  /**
   * @brief Current free size in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t free_size;

  /**
   * @brief Minimum free size ever in bytes.
   *
   * This value is an integral multiple of the page size.
   */
  uintptr_t min_free_size;

  /**
   * @brief Current number of free blocks.
   */
  uint32_t free_blocks;

  /**
   * @brief Maximum number of free blocks ever.
   */
  uint32_t max_free_blocks;

  /**
   * @brief Current number of used blocks.
   */
  uint32_t used_blocks;

  /**
   * @brief Maximum number of blocks searched ever.
   */
  uint32_t max_search;

  /**
   * @brief Total number of successful allocations.
   */
  uint32_t allocs;

  /**
   * @brief Total number of searches ever.
   */
  uint32_t searches;

  /**
   * @brief Total number of suceessful calls to free.
   */
  uint32_t frees;

  /**
   * @brief Total number of successful resizes.
   */
  uint32_t resizes;
} Heap_Statistics;

/**
 * @brief Control block used to manage a heap.
 */
struct Heap_Control {
  Heap_Block free_list;
  uintptr_t page_size;
  uintptr_t min_block_size;
  uintptr_t area_begin;
  uintptr_t area_end;
  Heap_Block *first_block;
  Heap_Block *last_block;
  Heap_Statistics stats;
  #ifdef HEAP_PROTECTION
    Heap_Protection Protection;
  #endif
};

/**
 * @brief Information about blocks.
 */
typedef struct {
  /**
   * @brief Number of blocks of this type.
   */
  uint32_t number;

  /**
   * @brief Largest block of this type.
   */
  uint32_t largest;

  /**
   * @brief Total size of the blocks of this type.
   */
  uint32_t total;
} Heap_Information;

/**
 * @brief Information block returned by _Heap_Get_information().
 */
typedef struct {
  Heap_Information Free;
  Heap_Information Used;
} Heap_Information_block;

/**
 * @brief See _Heap_Resize_block().
 */
typedef enum {
  HEAP_RESIZE_SUCCESSFUL,
  HEAP_RESIZE_UNSATISFIED,
  HEAP_RESIZE_FATAL_ERROR
} Heap_Resize_status;

/**
 * @brief Heap area structure for table based heap initialization and
 * extension.
 *
 * @see Heap_Initialization_or_extend_handler.
 */
typedef struct {
  void *begin;
  uintptr_t size;
} Heap_Area;

/**
 * @brief Heap initialization and extend handler type.
 *
 * This helps to do a table based heap initialization and extension.  Create a
 * table of Heap_Area elements and iterate through it.  Set the handler to
 * _Heap_Initialize() in the first iteration and then to _Heap_Extend().
 *
 * @see Heap_Area, _Heap_Initialize(), _Heap_Extend(), or _Heap_No_extend().
 */
typedef uintptr_t (*Heap_Initialization_or_extend_handler)(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size_or_unused
);

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
 * @brief Extends the memory available for the heap @a heap using the memory
 * area starting at @a area_begin of size @a area_size bytes.
 *
 * There are no alignment requirements.  The memory area must be big enough to
 * contain some maintainance blocks.  It must not overlap parts of the current
 * heap areas.  Disconnected subordinate heap areas will lead to used blocks
 * which cover the gaps.  Extending with an inappropriate memory area will
 * corrupt the heap.
 *
 * The unused fourth parameter is provided to have the same signature as
 * _Heap_Initialize().
 *
 * Returns the extended space available for allocation, or zero in case of failure.
 *
 * @see Heap_Initialization_or_extend_handler.
 */
uintptr_t _Heap_Extend(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t unused
);

/**
 * @brief This function returns always zero.
 *
 * This function only returns zero and does nothing else.
 *
 * Returns always zero.
 *
 * @see Heap_Initialization_or_extend_handler.
 */
uintptr_t _Heap_No_extend(
  Heap_Control *unused_0,
  void *unused_1,
  uintptr_t unused_2,
  uintptr_t unused_3
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

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Align_up(
  uintptr_t value,
  uintptr_t alignment
)
{
  uintptr_t remainder = value % alignment;

  if ( remainder != 0 ) {
    return value - remainder + alignment;
  } else {
    return value;
  }
}

/**
 * @brief Returns the worst case overhead to manage a memory area.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Area_overhead(
  uintptr_t page_size
)
{
  if ( page_size != 0 ) {
    page_size = _Heap_Align_up( page_size, CPU_ALIGNMENT );
  } else {
    page_size = CPU_ALIGNMENT;
  }

  return 2 * (page_size - 1) + HEAP_BLOCK_HEADER_SIZE;
}

#if !defined(__RTEMS_APPLICATION__)

#include <rtems/score/heap.inl>

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

/** @} */

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

#endif /* !defined(__RTEMS_APPLICATION__) */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
