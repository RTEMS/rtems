/** 
 *  @file heap.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the heap handler.
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

#ifndef __HEAP_inl
#define __HEAP_inl

/**
 *  @addtogroup ScoreHeap 
 *  @{
 */

#include <rtems/score/address.h>

/**
 *  This function returns the head of the specified heap.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Head (
  Heap_Control *the_heap
)
{
  return (Heap_Block *)&the_heap->start;
}

/**
 *  This function returns the tail of the specified heap.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Tail (
  Heap_Control *the_heap
)
{
  return (Heap_Block *)&the_heap->final;
}

/**
 *  This function returns the address of the block which physically
 *  precedes the_block in memory.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Previous_block (
  Heap_Block *the_block
)
{
  return (Heap_Block *) _Addresses_Subtract_offset(
                          (void *)the_block,
                          the_block->back_flag & ~ HEAP_BLOCK_USED
                        );
}

/**
 *  This function returns the address of the block which physically
 *  follows the_block in memory.
 *
 *  @note Next_block assumes that the block is free.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Next_block (
  Heap_Block *the_block
)
{
  return (Heap_Block *) _Addresses_Add_offset(
                          (void *)the_block,
                          the_block->front_flag & ~ HEAP_BLOCK_USED
                        );
}

/**
 *  This function calculates and returns a block's location (address)
 *  in the heap based upon a base address and an offset.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  void       *base,
  uint32_t    offset
)
{
  return (Heap_Block *) _Addresses_Add_offset( (void *)base, offset );
}

/**
 *  XXX
 */
 
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_User_block_at(
  void       *base
)
{
  uint32_t           offset;
 
  offset = *(((uint32_t   *) base) - 1);
  return _Heap_Block_at( base, -offset + -HEAP_BLOCK_USED_OVERHEAD);
}

/**
 *  This function returns TRUE if the previous block of the_block
 *  is free, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_previous_block_free (
  Heap_Block *the_block
)
{
  return !(the_block->back_flag & HEAP_BLOCK_USED);
}

/**
 *  This function returns TRUE if the block is free, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_block_free (
  Heap_Block *the_block
)
{
  return !(the_block->front_flag & HEAP_BLOCK_USED);
}

/**
 *  This function returns TRUE if the block is currently allocated,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_block_used (
  Heap_Block *the_block
)
{
  return (the_block->front_flag & HEAP_BLOCK_USED);
}

/**
 *  This function returns the size of the_block in bytes.
 */

RTEMS_INLINE_ROUTINE uint32_t   _Heap_Block_size (
  Heap_Block *the_block
)
{
  return (the_block->front_flag & ~HEAP_BLOCK_USED);
}

/**
 *  This function returns the starting address of the portion of the block
 *  which the user may access.
 */

RTEMS_INLINE_ROUTINE void *_Heap_Start_of_user_area (
  Heap_Block *the_block
)
{
  return (void *) &the_block->next;
}

/**
 *  This function returns TRUE if the_block is within the memory area
 *  managed by the_heap, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_block_in (
  Heap_Control *the_heap,
  Heap_Block   *the_block
)
{
  return _Addresses_Is_in_range( the_block, the_heap->start, the_heap->final );
}

/**
 *  This function validates a specified heap page size.  If the page size
 *  is 0 or if lies outside a page size alignment boundary it is invalid
 *  and FALSE is returned.  Otherwise, the page size is valid and TRUE is
 *  returned.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_page_size_valid(
  uint32_t   page_size
)
{
  return ((page_size != 0) &&
         ((page_size % CPU_HEAP_ALIGNMENT) == 0));
}

/**
 *  This function returns the block flag composed of size and in_use_flag.
 *  The flag returned is suitable for use as a back or front flag in a
 *  heap block.
 */

RTEMS_INLINE_ROUTINE uint32_t   _Heap_Build_flag (
  uint32_t   size,
  uint32_t   in_use_flag
)
{
  return  size | in_use_flag;
}

/**@}*/

#endif
/* end of include file */
