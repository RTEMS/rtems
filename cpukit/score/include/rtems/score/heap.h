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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_HEAP_H
#define _RTEMS_SCORE_HEAP_H

#include <rtems/score/cpu.h>
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
    uintptr_t delayed_free_fraction;
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
   * @brief Lifetime number of bytes allocated from this heap.
   *
   * This value is an integral multiple of the page size.
   */
  uint64_t lifetime_allocated;

  /**
   * @brief Lifetime number of bytes freed to this heap.
   *
   * This value is an integral multiple of the page size.
   */
  uint64_t lifetime_freed;

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
   * @brief Total number of searches.
   */
  uint32_t searches;

  /**
   * @brief Total number of successful allocations.
   */
  uint32_t allocs;

  /**
   * @brief Total number of failed allocations.
   */
  uint32_t failed_allocs;

  /**
   * @brief Total number of successful frees.
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
  Heap_Statistics Stats;
} Heap_Information_block;

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

RTEMS_INLINE_ROUTINE uintptr_t _Heap_Min_block_size( uintptr_t page_size )
{
  return _Heap_Align_up( sizeof( Heap_Block ), page_size );
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

/**
 * @brief Returns the size with administration and alignment overhead for one
 * allocation.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Heap_Size_with_overhead(
  uintptr_t page_size,
  uintptr_t size,
  uintptr_t alignment
)
{
  if ( page_size != 0 ) {
    page_size = _Heap_Align_up( page_size, CPU_ALIGNMENT );
  } else {
    page_size = CPU_ALIGNMENT;
  }

  if ( page_size < alignment ) {
    page_size = alignment;
  }

  return HEAP_BLOCK_HEADER_SIZE + page_size - 1 + size;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
