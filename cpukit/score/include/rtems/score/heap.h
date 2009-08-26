/**
 * @file
 *
 * Heap Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_HEAP_H
#define _RTEMS_SCORE_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreHeap Heap Handler
 *
 * The Heap Handler provides a heap.
 *
 * A heap is a doubly linked list of variable size blocks which are allocated
 * using the first fit method.  Garbage collection is performed each time a
 * block is returned to the heap by coalescing neighbor blocks.  Control
 * information for both allocated and free blocks is contained in the heap
 * area.  A heap control structure contains control information for the heap.
 *
 * FIXME: The alignment routines could be made faster should we require only
 * powers of two to be supported both for 'page_size' and for 'alignment'
 * arguments. However, both workspace and malloc heaps are initialized with
 * CPU_HEAP_ALIGNMENT as 'page_size', and while all the BSPs seem to use
 * CPU_ALIGNMENT (that is power of two) as CPU_HEAP_ALIGNMENT, for whatever
 * reason CPU_HEAP_ALIGNMENT is only required to be multiple of CPU_ALIGNMENT
 * and explicitly not required to be a power of two.
 *
 * There are two kinds of blocks.  One sort describes a free block from which
 * we can allocate memory.  The other blocks are used and contain allocated
 * memory.  The free blocks are accessible via a list of free blocks.
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
 *   <tr><td>heap->begin</td><td colspan=2>heap area begin address</td></tr>
 *   <tr><td>first_block->prev_size</td><td colspan=2>arbitrary value</td></tr>
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
 *     <td>second_block->prev_size</td><td colspan=2>size of first block</td>
 *   </tr>
 *   <tr>
 *     <td>second_block->size</td>
 *     <td colspan=2>arbitrary size | @c HEAP_PREV_BLOCK_FREE</td>
 *   </tr>
 *   <tr><td>heap->end</td><td colspan=2>heap area end address</td></tr>
 * </table>
 *
 * @{
 */

/**
 * @brief Description for free or used blocks.
 */
typedef struct Heap_Block {
  /**
   * @brief Size of the previous block or part of the allocated area of the
   * previous block.
   *
   * This field is only valid if the previous block is free.  This case is
   * indicated by a cleared @c HEAP_PREV_BLOCK_USED flag in the
   * @a size_and_flag field of the current block.
   */
  uintptr_t prev_size;

  /**
   * @brief Contains the size of the current block and a flag which indicates
   * if the previous block is free or used.
   *
   * If the flag @c HEAP_PREV_BLOCK_USED is set, then the previous block is
   * used, otherwise the previous block is free.  A used previous block may
   * claim the @a prev_size field for allocation.  This trick allows to
   * decrease the overhead in the used blocks by the size of the
   * @a prev_size field.  As sizes are always multiples of four, the two least
   * significant bits are always zero. We use one of them to store the flag.
   *
   * This field is always valid.
   */
  uintptr_t size_and_flag;

  /**
   * @brief Pointer to the next free block or part of the allocated area.
   *
   * This field is page size aligned and begins of the allocated area in case
   * the block is used.
   *
   * This field is only valid if the block is free and thus part of the free
   * block list.
   */
  struct Heap_Block *next;

  /**
   * @brief Pointer to the previous free block or part of the allocated area.
   *
   * This field is only valid if the block is free and thus part of the free
   * block list.
   */
  struct Heap_Block *prev;
} Heap_Block;

#define HEAP_PREV_BLOCK_USED ((uintptr_t) 1)

#define HEAP_PREV_BLOCK_FREE ((uintptr_t) 0)

/**
 * @brief Offset from the block begin up to the block size field.
 */
#define HEAP_BLOCK_SIZE_OFFSET (sizeof(uintptr_t))

/**
 * @brief Offset from the block begin up to the allocated area begin.
 */
#define HEAP_BLOCK_ALLOC_AREA_OFFSET (sizeof(uintptr_t) * 2)

#define HEAP_BLOCK_USED_OVERHEAD (sizeof(uintptr_t) * 2)

#define HEAP_LAST_BLOCK_OVERHEAD HEAP_BLOCK_ALLOC_AREA_OFFSET

/**
 *  Run-time heap statistics.
 *
 *  @note (double)searches/allocs gives mean number of searches per alloc while
 *        max_search gives maximum number of searches ever performed on a
 *        single call to alloc.
 *
 *  @note the statistics is always gathered. I believe the imposed overhead is
 *        rather small. Feel free to make it compile-time option if you think
 *        the overhead is too high for your application.
 */
typedef struct {
  /** instance number of this heap */
  uint32_t instance;
  /** the size of the memory for heap */
  uintptr_t size;
  /** current free size */
  uintptr_t free_size;
  /** minimum free size ever */
  uintptr_t min_free_size;
  /** current number of free blocks */
  uint32_t free_blocks;
  /** maximum number of free blocks ever */
  uint32_t max_free_blocks;
  /** current number of used blocks */
  uint32_t used_blocks;
  /** maximum number of blocks searched ever */
  uint32_t max_search;
  /** total number of successful calls to alloc */
  uint32_t allocs;
  /** total number of searches ever */
  uint32_t searches;
  /** total number of suceessful calls to free */
  uint32_t frees;
  /** total number of successful resizes */
  uint32_t resizes;
} Heap_Statistics;

/**
 *  Control block used to manage each heap.
 */
typedef struct {
  /** head and tail of circular list of free blocks */
  Heap_Block free_list;
  /** allocation unit and alignment */
  uintptr_t page_size;
  /** minimum block size aligned on page_size */
  uintptr_t min_block_size;
  /** first address of memory for the heap */
  uintptr_t begin;
  /** first address past end of memory for the heap */
  uintptr_t end;
  /** first valid block address in the heap */
  Heap_Block *start;
  /** last valid block address in the heap */
  Heap_Block *final;
  /** run-time statistics */
  Heap_Statistics stats;
} Heap_Control;

/**
 *  Status codes for _Heap_Extend
 */
typedef enum {
  HEAP_EXTEND_SUCCESSFUL,
  HEAP_EXTEND_ERROR,
  HEAP_EXTEND_NOT_IMPLEMENTED
} Heap_Extend_status;

/**
 *  Status codes for _Heap_Resize_block
 */
typedef enum {
  HEAP_RESIZE_SUCCESSFUL,
  HEAP_RESIZE_UNSATISFIED,
  HEAP_RESIZE_FATAL_ERROR
} Heap_Resize_status;

/**
 *  Status codes for _Heap_Get_information
 */
typedef enum {
  HEAP_GET_INFORMATION_SUCCESSFUL = 0,
  HEAP_GET_INFORMATION_BLOCK_ERROR
} Heap_Get_information_status;

/**
 *  Information block returned by the Heap routines used to
 *  obtain heap information.  This information is returned about
 *  either free or used blocks.
 */
typedef struct {
  /** Number of blocks of this type. */
  uint32_t number;
  /** Largest blocks of this type. */
  uint32_t largest;
  /** Total size of the blocks of this type. */
  uint32_t total;
} Heap_Information;

/**
 *  Information block returned by _Heap_Get_information
 */
typedef struct {
  /** This field is information on the used blocks in the heap. */
  Heap_Information Free;
  /** This field is information on the used blocks in the heap. */
  Heap_Information Used;
} Heap_Information_block;

/**
 * Initializes the @a heap control block to manage the area starting at
 * @a area_begin of @a area_size bytes.
 *
 * Blocks of memory are allocated from the heap in multiples of @a page_size
 * byte units.  If the @a page_size is equal to zero or is not multiple of
 * @c CPU_ALIGNMENT, it is aligned up to the nearest @c CPU_ALIGNMENT boundary.
 *
 * Returns the maximum memory available, or zero in case of failure.
 */
uintptr_t _Heap_Initialize(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_size
);

/**
 *  This routine grows @a heap memory area using the size bytes which
 *  begin at @a starting_address.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the memory
 *         to add to the heap
 *  @param[in] size is the size in bytes of the memory area to add
 *  @param[in] amount_extended points to a user area to return the
 *  @return a status indicating success or the reason for failure
 *  @return *size filled in with the amount of memory added to the heap
 */
Heap_Extend_status _Heap_Extend(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t *amount_extended
);

/**
 *  This function attempts to allocate a block of @a size bytes from
 *  @a heap.  If insufficient memory is free in @a heap to allocate
 *  a block of the requested size, then NULL is returned.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] size is the amount of memory to allocate in bytes
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Heap_Allocate( Heap_Control *heap, uintptr_t size );

/**
 *  This function attempts to allocate a memory block of @a size bytes from
 *  @a heap so that the start of the user memory is aligned on the
 *  @a alignment boundary. If @a alignment is 0, it is set to CPU_ALIGNMENT.
 *  Any other value of @a alignment is taken "as is", i.e., even odd
 *  alignments are possible.
 *  Returns pointer to the start of the memory block if success, NULL if
 *  failure.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] size is the amount of memory to allocate in bytes
 *  @param[in] alignment the required alignment
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Heap_Allocate_aligned(
  Heap_Control *heap,
  uintptr_t size,
  uintptr_t alignment
);

/**
 *  This function sets @a size to the size of the block of allocatable area
 *  which begins at @a starting_address. The size returned in @a *size could
 *  be greater than the size requested for allocation.
 *  Returns true if the @a starting_address is in the heap, and false
 *  otherwise.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the user block
 *         to obtain the size of
 *  @param[in] size points to a user area to return the size in
 *  @return true if successfully able to determine the size, false otherwise
 *  @return *size filled in with the size of the user area for this block
 */
bool _Heap_Size_of_alloc_area(
  Heap_Control *heap,
  void *area_begin,
  uintptr_t *size
);

/**
 *  This function tries to resize in place the block that is pointed to by the
 *  @a starting_address to the new @a size.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the user block
 *         to be resized
 *  @param[in] size is the new size
 *  @param[in] old_mem_size points to a user area to return the size of the
 *         user memory area of the block before resizing.
 *  @param[in] avail_mem_size points to a user area to return the size of
 *         the user memory area of the free block that has been enlarged or
 *         created due to resizing, 0 if none.
 *  @return HEAP_RESIZE_SUCCESSFUL if successfully able to resize the block,
 *          HEAP_RESIZE_UNSATISFIED if the block can't be resized in place,
 *          HEAP_RESIZE_FATAL_ERROR if failure
 *  @return *old_mem_size filled in with the size of the user memory area of
 *          the block before resizing.
 *  @return *avail_mem_size filled in with the size of the user memory area
 *          of the free block that has been enlarged or created due to
 *          resizing, 0 if none.
 */
Heap_Resize_status _Heap_Resize_block(
  Heap_Control *heap,
  void         *starting_address,
  uintptr_t      size,
  uintptr_t     *old_mem_size,
  uintptr_t     *avail_mem_size
);

/**
 *  This routine returns the block of memory which begins
 *  at @a alloc_area_begin to @a heap.  Any coalescing which is
 *  possible with the freeing of this routine is performed.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] start_address is the starting address of the user block
 *         to free
 *  @return true if successfully freed, false otherwise
 */
bool _Heap_Free( Heap_Control *heap, void *alloc_area_begin );

/**
 *  This routine walks the heap to verify its integrity.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] source is a user specified integer which may be used to
 *         indicate where in the application this was invoked from
 *  @param[in] do_dump is set to true if errors should be printed
 *  @return true if the test passed fine, false otherwise.
 */
bool _Heap_Walk(
  Heap_Control *heap,
  int           source,
  bool          do_dump
);

/**
 *  This routine walks the heap and tots up the free and allocated
 *  sizes.
 *
 *  @param[in] heap pointer to heap header
 *  @param[in] the_info pointer to a status information area
 *  @return *the_info is filled with status information
 *  @return 0=success, otherwise heap is corrupt.
 */
Heap_Get_information_status _Heap_Get_information(
  Heap_Control            *heap,
  Heap_Information_block  *the_info
);

/**
 *  This heap routine returns information about the free blocks
 *  in the specified heap.
 *
 *  @param[in] heap pointer to heap header.
 *  @param[in] info pointer to the free block information.
 *
 *  @return free block information filled in.
 */
void _Heap_Get_free_information(
  Heap_Control        *heap,
  Heap_Information    *info
);

#if !defined(__RTEMS_APPLICATION__)

#include <rtems/score/heap.inl>

/**
 * @brief Returns the minimal block size for a block which may contain an area
 * of size @a alloc_size for allocation, or zero in case of an overflow.
 *
 * Uses the heap values @a page_size and @a min_block_size.
 */
uintptr_t _Heap_Calc_block_size(
  uintptr_t alloc_size,
  uintptr_t page_size,
  uintptr_t min_block_size
);

/**
 *  This method allocates a block of size @a alloc_size from @a the_block
 *  belonging to @a heap. Split @a the_block if possible, otherwise
 *  allocate it entirely.  When split, make the lower part used, and leave
 *  the upper part free.
 *
 *  This is an internal routines used by _Heap_Allocate() and
 *  _Heap_Allocate_aligned().  Refer to 'heap.c' for details.
 *
 *  @param[in] heap is the heap to operate upon
 *  @param[in] the_block is the block to allocates the requested size from
 *  @param[in] alloc_size is the requested number of bytes to take out of 
 *         the block
 *
 *  @return This methods returns the size of the allocated block.
 */
uintptr_t _Heap_Block_allocate(
  Heap_Control *heap,
  Heap_Block *block,
  uintptr_t alloc_size
);

/** @} */

#ifdef RTEMS_DEBUG
  #define RTEMS_HEAP_DEBUG
  #define RTEMS_MALLOC_BOUNDARY_HELPERS
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
