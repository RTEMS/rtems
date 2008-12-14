/**
 *  @file  rtems/score/protectedheap.h
 *
 *  This include file contains the information pertaining to the 
 *  Protected Heap Handler.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_PROTECTED_HEAP_H
#define _RTEMS_SCORE_PROTECTED_HEAP_H

#include <rtems/score/heap.h>
#include <rtems/score/apimutex.h>

/**
 *  @defgroup ScoreProtHeap Protected Heap Handler
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Protected Heap services.
 *
 *  It is a simple wrapper for the help with the addition of the
 *  allocation mutex being used for protection.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This routine initializes @a the_heap record to manage the
 *  contiguous heap of @a size bytes which starts at @a starting_address.
 *  Blocks of memory are allocated from the heap in multiples of
 *  @a page_size byte units. If @a page_size is 0 or is not multiple of
 *  CPU_ALIGNMENT, it's aligned up to the nearest CPU_ALIGNMENT boundary.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the memory for
 *         the heap
 *  @param[in] size is the size in bytes of the memory area for the heap
 *  @param[in] page_size is the size in bytes of the allocation unit
 *
 *  @return This method returns the maximum memory available.  If
 *          unsuccessful, 0 will be returned.
 */
static inline uint32_t _Protected_heap_Initialize(
  Heap_Control *the_heap,
  void         *starting_address,
  intptr_t      size,
  uint32_t      page_size
)
{
  return _Heap_Initialize( the_heap, starting_address, size, page_size );
}

/**
 *  This routine grows @a the_heap memory area using the size bytes which
 *  begin at @a starting_address.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the memory
 *         to add to the heap
 *  @param[in] size is the size in bytes of the memory area to add
 *  @return a status indicating success or the reason for failure
 */
bool _Protected_heap_Extend(
  Heap_Control *the_heap,
  void         *starting_address,
  intptr_t      size
);

/**
 *  This function attempts to allocate a block of @a size bytes from
 *  @a the_heap.  If insufficient memory is free in @a the_heap to allocate
 *  a block of the requested size, then NULL is returned.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] size is the amount of memory to allocate in bytes
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Protected_heap_Allocate(
  Heap_Control *the_heap,
  intptr_t      size
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
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] size is the amount of memory to allocate in bytes
 *  @param[in] alignment the required alignment
 *  @return NULL if unsuccessful and a pointer to the block if successful
 */
void *_Protected_heap_Allocate_aligned(
  Heap_Control *the_heap,
  intptr_t      size,
  uint32_t      alignment
);

/**
 *  This function sets @a *size to the size of the block of user memory
 *  which begins at @a starting_address. The size returned in @a *size could
 *  be greater than the size requested for allocation.
 *  Returns TRUE if the @a starting_address is in the heap, and FALSE
 *  otherwise.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the user block
 *         to obtain the size of
 *  @param[in] size points to a user area to return the size in
 *  @return TRUE if successfully able to determine the size, FALSE otherwise
 *  @return *size filled in with the size of the user area for this block
 */
bool _Protected_heap_Get_block_size(
  Heap_Control        *the_heap,
  void                *starting_address,
  intptr_t            *size
);

/**
 *  This function tries to resize in place the block that is pointed to by the
 *  @a starting_address to the new @a size.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] starting_address is the starting address of the user block
 *         to be resized
 *  @param[in] size is the new size
 *
 *  @return TRUE if successfully able to resize the block.
 *          FALSE if the block can't be resized in place.
 */
bool _Protected_heap_Resize_block(
  Heap_Control *the_heap,
  void         *starting_address,
  intptr_t      size
);

/**
 *  This routine returns the block of memory which begins
 *  at @a starting_address to @a the_heap.  Any coalescing which is
 *  possible with the freeing of this routine is performed.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] start_address is the starting address of the user block
 *         to free
 *  @return TRUE if successfully freed, FALSE otherwise
 */
bool _Protected_heap_Free(
  Heap_Control *the_heap,
  void         *start_address
);

/**
 *  This routine walks the heap to verify its integrity.
 *
 *  @param[in] the_heap is the heap to operate upon
 *  @param[in] source is a user specified integer which may be used to
 *         indicate where in the application this was invoked from
 *  @param[in] do_dump is set to TRUE if errors should be printed
 *  @return TRUE if the test passed fine, FALSE otherwise.
 */
bool _Protected_heap_Walk(
  Heap_Control *the_heap,
  int           source,
  bool          do_dump
);

/**
 *  This routine walks the heap and tots up the free and allocated
 *  sizes.
 *
 *  @param[in] the_heap pointer to heap header
 *  @param[in] the_info pointer to a status information area
 */
void _Protected_heap_Get_information(
  Heap_Control            *the_heap,
  Heap_Information_block  *the_info
);

/**
 *  This heap routine returns information about the free blocks
 *  in the specified heap.
 *
 *  @param[in] the_heap pointer to heap header.
 *  @param[in] info pointer to the free block information.
 *
 *  @return free block information filled in.
 */
void _Protected_heap_Get_free_information(
  Heap_Control        *the_heap,
  Heap_Information    *info
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
