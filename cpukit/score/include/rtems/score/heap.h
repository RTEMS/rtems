/**
 *  @file  rtems/score/heap.h
 *
 *  This include file contains the information pertaining to the Heap
 *  Handler.  A heap is a doubly linked list of variable size
 *  blocks which are allocated using the first fit method.  Garbage
 *  collection is performed each time a block is returned to the heap by
 *  coalescing neighbor blocks.  Control information for both allocated
 *  and unallocated blocks is contained in the heap space.  A heap control
 *  structure contains control information for the heap.
 *
 *  FIXME: the alignment routines could be made faster should we require only
 *         powers of two to be supported both for 'page_size' and for
 *         'alignment' arguments. However, both workspace and malloc heaps are
 *         initialized with CPU_HEAP_ALIGNMENT as 'page_size', and while all
 *         the BSPs seem to use CPU_ALIGNMENT (that is power of two) as
 *         CPU_HEAP_ALIGNMENT, for whatever reason CPU_HEAP_ALIGNMENT is only
 *         required to be multiple of CPU_ALIGNMENT and explicitly not
 *         required to be a power of two.
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_HEAP_h
#define __RTEMS_HEAP_h

/**
 *  @defgroup ScoreHeap Heap Handler
 *
 *  This group contains functionality which provides the foundation
 *  Heap services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This type defines unsigned integer type to store 'void*'. Analog of C99
 * 'uintptr_t'. This should work on 16/32/64 bit architectures.
 *
 * FIXME: Something like this should better be defined by
 *        'rtems/score/types.h' and used here.
 */

typedef unsigned long int _H_uptr_t;

/**
 *  Forward reference
 *
 *  @ref Heap_Block
 */
typedef struct Heap_Block_struct Heap_Block;

/**
 *  The following defines the data structure used to manage individual blocks
 *  in a heap.  When the block is allocated, the 'next' and 'prev' fields, as
 *  well as 'prev_size' field of the next block, are not used by the heap
 *  manager and thus the address returned for the block starts at the address
 *  of the 'next' field and the size of the user accessible area includes the
 *  size of the 'prev_size' field.
 *
 *  @note The 'next' and 'prev' pointers are only valid when the block is free.
 *     Caution must be taken to ensure that every block is large enough to
 *     hold them and that they are not accessed while the block is actually
 *     allocated (i.e., not free).
 *
 *  @note The 'prev_size' field is only valid when HEAP_PREV_USED bit is clear
 *     in the 'size' field indicating that previous block is not allocated.
 *     If the bit is set, the 'prev_size' field is part of user-accessible
 *     space of the previous allocated block and thus shouldn't be accessed
 *     by the heap manager code. This trick allows to further decrease
 *     overhead in the used blocks to the size of 'size' field (4 bytes).
 *
 */

struct Heap_Block_struct {
  /** size of prev block (if prev block is free) */
  uint32_t  prev_size;
  /** size of block in bytes and status of prev block */
  uint32_t  size;
  /** pointer to the next free block */
  Heap_Block *next;
  /** pointer to the previous free block */
  Heap_Block *prev;
};

/**
 *  This flag used in the 'size' field of each heap block to indicate
 *  if previous block is free or in use. As sizes are always multiples of
 *  4, the 2 least significant bits would always be 0, and we use one of them
 *  to store the flag.
 */

#define HEAP_PREV_USED    1     /* indicates previous block is in use */

/**
 *  The following constants reflect various requirements of the
 *  heap data structures which impact the management of a heap.
 */

#define HEAP_MIN_BLOCK_SIZE (sizeof(Heap_Block))

/**
 *  Offset of the block header from the block pointer. Equal to the
 *  offsetof(Heap_Block.size).
 */
#define HEAP_BLOCK_HEADER_OFFSET (sizeof(uint32_t))

/**
 *  Offset of user data pointer from the block pointer. Equal to the
 *  offsetof(Heap_Block.next).
 */
#define HEAP_BLOCK_USER_OFFSET (sizeof(uint32_t) * 2)

/**
 *  Num bytes of overhead in used block. Equal to the sizeof(Heap_Block.size).
 */
#define HEAP_BLOCK_USED_OVERHEAD \
  (HEAP_BLOCK_USER_OFFSET - HEAP_BLOCK_HEADER_OFFSET)

/** Size of the permanent dummy last block. */
#define HEAP_OVERHEAD HEAP_BLOCK_USER_OFFSET

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

typedef struct Heap_Statistics_tag {
  /** instance number of this heap */
  uint32_t instance;
  /** the size of the memory for heap */
  uint32_t size;
  /** current free size */
  uint32_t free_size;
  /** minimum free size ever */
  uint32_t min_free_size;
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
} Heap_Statistics;

/**
 *  Control block used to manage each heap.
 */
typedef struct {
  /** head and tail of circular list of free blocks */
  Heap_Block  free_list;
  /** allocation unit and alignment */
  uint32_t  page_size;
  /** minimum block size aligned on page_size */
  uint32_t  min_block_size;
  /** first address of memory for the heap */
  void       *begin;
  /** first address past end of memory for the heap */
  void       *end;
  /** first valid block address in the heap */
  Heap_Block *start;
  /** last valid block address in the heap */
  Heap_Block *final;
  /** run-time statistics */
  Heap_Statistics stats;
} Heap_Control;

/**
 *  Status codes for heap_extend
 */

typedef enum {
  HEAP_EXTEND_SUCCESSFUL,
  HEAP_EXTEND_ERROR,
  HEAP_EXTEND_NOT_IMPLEMENTED
} Heap_Extend_status;

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
 *  This routine initializes @a the_heap record to manage the
 *  contiguous heap of @a size bytes which starts at @a starting_address.
 *  Blocks of memory are allocated from the heap in multiples of
 *  @a page_size byte units. If @a page_size is 0 or is not multiple of
 *  CPU_ALIGNMENT, it's aligned up to the nearest CPU_ALIGNMENT boundary.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param starting_address (in) is the starting address of the memory for
 *         the heap
 *  @param size (in) is the size in bytes of the memory area for the heap
 *  @param page_size (in) is the size in bytes of the allocation unit
 *  @return XXX
 */
uint32_t   _Heap_Initialize(
  Heap_Control *the_heap,
  void         *starting_address,
  uint32_t      size,
  uint32_t      page_size
);

/**
 *  This routine grows @a the_heap memory area using the size bytes which
 *  begin at @a starting_address.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param starting_address (in) is the starting address of the memory
 *         to add to the heap
 *  @param size (in) is the size in bytes of the memory area to add
 *  @param amount_extended (in) points to a user area to return the
 *  @return a status indicating success or the reason for failure
 *  @return *size filled in with the amount of memory added to the heap
 */
Heap_Extend_status _Heap_Extend(
  Heap_Control *the_heap,
  void         *starting_address,
  uint32_t      size,
  uint32_t     *amount_extended
);

/**
 *  This function attempts to allocate a block of @a size bytes from
 *  @a the_heap.  If insufficient memory is free in @a the_heap to allocate
 *  a block of the requested size, then NULL is returned.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param size (in) is the amount of memory to allocate in bytes
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Heap_Allocate(
  Heap_Control *the_heap,
  uint32_t    size
);

/**
 *  This function attempts to allocate a memory block of @a size bytes from
 *  @a the_heap so that the start of the user memory is aligned on the
 *  @a alignment boundary. If @a alignment is 0, it is set to CPU_ALIGNMENT.
 *  Any other value of @a alignment is taken "as is", i.e., even odd
 *  alignments are possible.
 *  Returns pointer to the start of the memory block if success, NULL if
 *  failure.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param size (in) is the amount of memory to allocate in bytes
 *  @param alignment (in) the required alignment
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Heap_Allocate_aligned(
  Heap_Control *the_heap,
  uint32_t    size,
  uint32_t    alignment
);

/**
 *  This function sets @a *size to the size of the block of user memory
 *  which begins at @a starting_address. The size returned in @a *size could
 *  be greater than the size requested for allocation.
 *  Returns TRUE if the @a starting_address is in the heap, and FALSE
 *  otherwise.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param starting_address (in) is the starting address of the user block
 *         to obtain the size of
 *  @param size (in) points to a user area to return the size in
 *  @return TRUE if successfully able to determine the size, FALSE otherwise
 *  @return *size filled in with the size of the user area for this block
 */
boolean _Heap_Size_of_user_area(
  Heap_Control        *the_heap,
  void                *starting_address,
  size_t              *size
);

/**
 *  This routine returns the block of memory which begins
 *  at @a starting_address to @a the_heap.  Any coalescing which is
 *  possible with the freeing of this routine is performed.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param starting_address (in) is the starting address of the user block
 *         to free
 *  @return TRUE if successfully freed, FALSE otherwise
 */
boolean _Heap_Free(
  Heap_Control *the_heap,
  void         *start_address
);

/**
 *  This routine walks the heap to verify its integrity.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param source (in) XXX
 *  @param do_dump (in) is set to TRUE if errors should be printed
 *  @return TRUE if the test passed fine, FALSE otherwise.
 */

boolean _Heap_Walk(
  Heap_Control *the_heap,
  int           source,
  boolean       do_dump
);

/**
 *  This routine walks the heap and tots up the free and allocated
 *  sizes.
 *
 *  @param the_heap (in) pointer to heap header
 *  @param the_info (in) pointer to a status information area
 *  @return *the_info is filled with status information
 *  @return 0=success, otherwise heap is corrupt.
 */
Heap_Get_information_status _Heap_Get_information(
  Heap_Control            *the_heap,
  Heap_Information_block  *the_info
);

/**
 *  This heap routine returns information about the free blocks
 *  in the specified heap.
 *
 *  @param the_heap (in) pointer to heap header.
 *  @param info (in) pointer to the free block information.
 *
 *  @return free block information filled in.
 */

void _Heap_Get_free_information(
  Heap_Control        *the_heap,
  Heap_Information    *info
);

#if !defined(__RTEMS_APPLICATION__)

/*
 *  A pointer to unsigned integer conversion.
 */
#define _H_p2u(_p) ((_H_uptr_t)(_p))

#include <rtems/score/heap.inl>

/*
 *  Internal routines shared by _Heap_Allocate() and _Heap_Allocate_aligned().
 *  Refer to 'heap.c' for details.
 */

extern uint32_t _Heap_Calc_block_size(
  uint32_t size,
  uint32_t page_size,
  uint32_t min_size);

extern Heap_Block* _Heap_Block_allocate(
  Heap_Control* the_heap,
  Heap_Block* the_block,
  uint32_t alloc_size);

/*
 * Debug support
 */

#if defined(RTEMS_DEBUG)
#define RTEMS_HEAP_DEBUG
#endif

#if defined(RTEMS_HEAP_DEBUG)

#include <assert.h>

/*
 *  We do asserts only for heaps with instance number greater than 0 assuming
 *  that the heap used for malloc is initialized first and thus has instance
 *  number 0. Asserting malloc heap may lead to troubles as printf may invoke
 *  malloc thus probably leading to infinite recursion.
 */

#define _HAssert(cond_) \
  do { \
    if(the_heap->stats.instance && !(cond_)) \
      __assert(__FILE__, __LINE__, #cond_);  \
  } while(0)

#else  /* !defined(RTEMS_HEAP_DEBUG) */

#define _HAssert(cond_) ((void)0)

#endif /* !defined(RTEMS_HEAP_DEBUG) */

#endif /* !defined(__RTEMS_APPLICATION__) */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
