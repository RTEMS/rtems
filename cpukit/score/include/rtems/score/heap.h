/*  heap.h
 *
 *  This include file contains the information pertaining to the Heap
 *  Handler.  A heap is a doubly linked list of variable size
 *  blocks which are allocated using the first fit method.  Garbage
 *  collection is performed each time a block is returned to the heap by
 *  coalescing neighbor blocks.  Control information for both allocated
 *  and unallocated blocks is contained in the heap space.  A heap header
 *  contains control information for the heap.
 *
 *  COPYRIGHT (c) 1989-1999.
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

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Status codes for heap_extend
 */

typedef enum {
  HEAP_EXTEND_SUCCESSFUL,
  HEAP_EXTEND_ERROR,
  HEAP_EXTEND_NOT_IMPLEMENTED
}  Heap_Extend_status;

/*
 *  Status codes for _Heap_Get_information
 */

typedef enum {
    HEAP_GET_INFORMATION_SUCCESSFUL = 0,
    HEAP_GET_INFORMATION_SYSTEM_STATE_ERROR,
    HEAP_GET_INFORMATION_BLOCK_ERROR
}  Heap_Get_information_status;
        
/*
 *  Information block returned by _Heap_Get_information
 */

typedef struct {
  unsigned32   free_blocks;
  unsigned32   free_size;
  unsigned32   used_blocks;
  unsigned32   used_size;
} Heap_Information_block;

/*
 *  Constants used in the size/used field of each heap block to
 *  indicate when a block is free or in use.
 */

#define HEAP_BLOCK_USED    1           /* indicates block is in use */
#define HEAP_BLOCK_FREE    0           /* indicates block is free */

/*
 *  The size/used field value for the dummy front and back flags.
 */

#define HEAP_DUMMY_FLAG    (0 + HEAP_BLOCK_USED)

/*
 *  The following constants reflect various requirements of the
 *  heap data structures which impact the management of a heap.
 *
 * NOTE:   Because free block overhead is greater than used block
 *         overhead AND a portion of the allocated space is from
 *         the extra free block overhead, the absolute lower bound
 *         of the minimum fragment size is equal to the size of
 *         the free block overhead.
 */

#define HEAP_OVERHEAD \
  (sizeof( unsigned32 ) * 2)         /* size dummy first and last blocks */
#define HEAP_BLOCK_USED_OVERHEAD \
  (sizeof( void * ) * 2)             /* num bytes overhead in used block */
#define HEAP_MINIMUM_SIZE \
  (HEAP_OVERHEAD + sizeof (Heap_Block))
                                     /* min number of bytes the user may */
                                     /*   specify for the heap size      */

/*
 *  The following defines the data structure used to manage
 *  individual blocks in a heap.   When the block is allocated, the
 *  next and previous fields are not used by the Heap Handler
 *  and thus the address returned for the block starts at
 *  the address of the next field.
 *
 *  NOTE:  The next and previous pointers are only valid when the
 *         block is free.  Caution must be exercised to insure that
 *         allocated blocks are large enough to contain them and
 *         that they are not accidentally overwritten when the
 *         block is actually allocated.
 */

typedef struct Heap_Block_struct Heap_Block;

struct Heap_Block_struct {
  unsigned32  back_flag;   /* size and status of prev block */
  unsigned32  front_flag;  /* size and status of block */
  Heap_Block *next;        /* pointer to next block */
  Heap_Block *previous;    /* pointer to previous block */
};

/*
 *  The following defines the control block used to manage each heap.
 *
 *  NOTE:
 *
 *  This structure is layed out such that it can be used a a dummy
 *  first and last block on the free block chain.  The extra padding
 *  insures the dummy last block is the correct size.
 *
 *  The first Heap_Block starts at first while the second starts at
 *  final.  This is effectively the same trick as is used in the Chain
 *  Handler.
 */

typedef struct {
  Heap_Block *start;       /* first valid block address in heap */
  Heap_Block *final;       /* last valid block address in heap */

  Heap_Block *first;       /* pointer to first block in heap */
  Heap_Block *permanent_null;  /* always NULL pointer */
  Heap_Block *last;        /* pointer to last block in heap */
  unsigned32  page_size;   /* allocation unit */
  unsigned32  reserved;
}   Heap_Control;

/*
 *  _Heap_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_heap record to manage the
 *  contiguous heap of size bytes which starts at starting_address.
 *  Blocks of memory are allocated from the heap in multiples of
 *  page_size byte units.
 */

unsigned32 _Heap_Initialize(
  Heap_Control *the_heap,
  void         *starting_address,
  unsigned32    size,
  unsigned32    page_size
);

/*
 *  _Heap_Extend
 *
 *  DESCRIPTION:
 *
 *  This routine grows the_heap memory area using the size bytes which
 *  begin at starting_address.
 */

Heap_Extend_status _Heap_Extend(
  Heap_Control *the_heap,
  void         *starting_address,
  unsigned32    size,
  unsigned32   *amount_extended
);

/*
 *  _Heap_Allocate
 *
 *  DESCRIPTION:
 *
 *  DESCRIPTION:
 *
 *  This function attempts to allocate a block of size bytes from
 *  the_heap.  If insufficient memory is free in the_heap to allocate
 *  a  block of the requested size, then NULL is returned.
 */

void *_Heap_Allocate(
  Heap_Control *the_heap,
  unsigned32    size
);

/*
 *  _Heap_Size_of_user_area
 *
 *  DESCRIPTION:
 *
 *  This kernel routine sets size to the size of the given heap block.
 *  It returns TRUE if the starting_address is in the heap, and FALSE
 *  otherwise.
 */

boolean _Heap_Size_of_user_area(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32          *size
);

/*
 *  _Heap_Free
 *
 *  DESCRIPTION:
 *
 *  This routine returns the block of memory which begins
 *  at starting_address to the_heap.  Any coalescing which is
 *  possible with the freeing of this routine is performed.
 */

boolean _Heap_Free(
  Heap_Control *the_heap,
  void         *start_address
);

/*
 *  _Heap_Walk
 *
 *  DESCRIPTION:
 *
 *  This routine walks the heap to verify its integrity.
 */

void _Heap_Walk(
  Heap_Control *the_heap,
  int           source,
  boolean       do_dump
);

/*PAGE
 *
 *  _Heap_Get_information
 *
 *  This kernel routine walks the heap and tots up the free and allocated
 *  sizes.  Derived from _Heap_Walk.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header
 *    the_info  - pointer to information block
 *
 *  Output parameters: 
 *    *the_info - status information
 *    return 0=success, otherwise heap is corrupt.
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

#endif
/* end of include file */
