/** 
 *  @file heap.h
 *
 *  This include file contains the information pertaining to the Heap
 *  Handler.  A heap is a doubly linked list of variable size
 *  blocks which are allocated using the first fit method.  Garbage
 *  collection is performed each time a block is returned to the heap by
 *  coalescing neighbor blocks.  Control information for both allocated
 *  and unallocated blocks is contained in the heap space.  A heap header
 *  contains control information for the heap.
 */

/*
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
 *  Status codes for heap_extend
 */
typedef enum {
  HEAP_EXTEND_SUCCESSFUL,
  HEAP_EXTEND_ERROR,
  HEAP_EXTEND_NOT_IMPLEMENTED
}  Heap_Extend_status;

/**
 *  Status codes for _Heap_Get_information
 */
typedef enum {
    HEAP_GET_INFORMATION_SUCCESSFUL = 0,
    HEAP_GET_INFORMATION_SYSTEM_STATE_ERROR,
    HEAP_GET_INFORMATION_BLOCK_ERROR
}  Heap_Get_information_status;

/**
 *  Information block returned by _Heap_Get_information
 */
typedef struct {
  /** This field is the number of free blocks in this heap. */
  uint32_t     free_blocks;
  /** This field is the amount of free memory in this heap. */
  uint32_t     free_size;
  /** This field is the number of used blocks in this heap. */
  uint32_t     used_blocks;
  /** This field is the amount of used memory in this heap. */
  uint32_t     used_size;
} Heap_Information_block;

/**
 *  This constant is used in the size/used field of each heap block to
 *  indicate when a block is in use.
 */
#define HEAP_BLOCK_USED    1

/**
 *  This constant is used in the size/used field of each heap block to
 *  indicate when a block is free.
 */
#define HEAP_BLOCK_FREE    0

/**
 *  The size/used field value for the dummy front and back flags.
 */
#define HEAP_DUMMY_FLAG    (0 + HEAP_BLOCK_USED)

/*
 *  The following constants reflect various requirements of the
 *  heap data structures which impact the management of a heap.
 *
 *  NOTE:  Because free block overhead is greater than used block
 *         overhead AND a portion of the allocated space is from
 *         the extra free block overhead, the absolute lower bound
 *         of the minimum fragment size is equal to the size of
 *         the free block overhead.
 */

/** size dummy first and last blocks */
#define HEAP_OVERHEAD (sizeof( uint32_t   ) * 2)

/** number of bytes of overhead in used block */
#define HEAP_BLOCK_USED_OVERHEAD (sizeof( void * ) * 2)

/** Minimum number of bytes the user may specify for the heap size */
#define HEAP_MINIMUM_SIZE (HEAP_OVERHEAD + sizeof (Heap_Block))

/**
 *  Forward reference
 *
 *  @ref Heap_Block
 */
typedef struct Heap_Block_struct Heap_Block;

/**
 *  The following defines the data structure used to manage
 *  individual blocks in a heap.   When the block is allocated, the
 *  next and previous fields are not used by the Heap Handler
 *  and thus the address returned for the block starts at
 *  the address of the next field.
 *
 *  @note  The next and previous pointers are only valid when the
 *         block is free.  Caution must be exercised to insure that
 *         allocated blocks are large enough to contain them and
 *         that they are not accidentally overwritten when the
 *         block is actually allocated.
 */
struct Heap_Block_struct {
  /** size and status of prev block */
  uint32_t    back_flag;
  /** size and status of block */
  uint32_t    front_flag;
  /** pointer to next block */
  Heap_Block *next;
  /** pointer to previous block */
  Heap_Block *previous;
};

/**
 *  The following defines the control block used to manage each heap.
 *
 *  @note This structure is layed out such that it can be used a a dummy
 *  first and last block on the free block chain.  The extra padding
 *  insures the dummy last block is the correct size.
 *
 *  The first Heap_Block starts at first while the second starts at
 *  final.  This is effectively the same trick as is used in the Chain
 *  Handler.
 */
typedef struct {
  /** first valid block address in heap */
  Heap_Block *start;
  /** last valid block address in heap */
  Heap_Block *final;

  /** pointer to first block in heap */
  Heap_Block *first;
  /** always NULL pointer */
  Heap_Block *permanent_null;
  /** pointer to last block in heap */
  Heap_Block *last;
  /** allocation unit */
  uint32_t    page_size;
  /** reserved field */
  uint32_t    reserved;
}   Heap_Control;

/**
 *  This routine initializes @a the_heap record to manage the
 *  contiguous heap of @a size bytes which starts at @a starting_address.
 *  Blocks of memory are allocated from the heap in multiples of
 *  @a page_size byte units.
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
 *  This function attempts to allocate a block of size bytes from
 *  @a the_heap.  If insufficient memory is free in @a the_heap to allocate
 *  a  block of the requested size, then NULL is returned.
 *
 *  @param the_heap (in) is the heap to operate upon
 *  @param size (in) is the amount of memory to allocate in bytes
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Heap_Allocate(
  Heap_Control *the_heap,
  uint32_t      size
);

/**
 *  This kernel routine sets size to the size of the given heap block.
 *  It returns TRUE if the @a starting_address is in @a the_heap, and FALSE
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
  uint32_t            *size
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
 */
void _Heap_Walk(
  Heap_Control *the_heap,
  int           source,
  boolean       do_dump
);

/**
 *  This kernel routine walks the heap and tots up the free and allocated
 *  sizes.  Derived from _Heap_Walk.
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


#ifndef __RTEMS_APPLICATION__
#include <rtems/score/heap.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
