/*  heap.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the heap handler.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __HEAP_inl
#define __HEAP_inl

#include <rtems/score/address.h>

/*PAGE
 *
 *  _Heap_Head
 *
 *  DESCRIPTION:
 *
 *  This function returns the head of the specified heap.
 */

STATIC INLINE Heap_Block *_Heap_Head (
  Heap_Control *the_heap
)
{
  return (Heap_Block *)&the_heap->start;
}

/*PAGE
 *
 *  _Heap_Tail
 *
 *  DESCRIPTION:
 *
 *  This function returns the tail of the specified heap.
 */

STATIC INLINE Heap_Block *_Heap_Tail (
  Heap_Control *the_heap
)
{
  return (Heap_Block *)&the_heap->final;
}

/*PAGE
 *
 *  _Heap_Previous_block
 *
 *  DESCRIPTION:
 *
 *  This function returns the address of the block which physically
 *  precedes the_block in memory.
 */

STATIC INLINE Heap_Block *_Heap_Previous_block (
  Heap_Block *the_block
)
{
  return (Heap_Block *) _Addresses_Subtract_offset(
                          (void *)the_block,
                          the_block->back_flag & ~ HEAP_BLOCK_USED
                        );
}

/*PAGE
 *
 *  _Heap_Next_block
 *
 *  DESCRIPTION:
 *
 *  This function returns the address of the block which physically
 *  follows the_block in memory.
 *
 *  NOTE: Next_block assumes that the block is free.
 */

STATIC INLINE Heap_Block *_Heap_Next_block (
  Heap_Block *the_block
)
{
  return (Heap_Block *) _Addresses_Add_offset(
                          (void *)the_block,
                          the_block->front_flag & ~ HEAP_BLOCK_USED
                        );
}

/*PAGE
 *
 *  _Heap_Block_at
 *
 *  DESCRIPTION:
 *
 *  This function calculates and returns a block's location (address)
 *  in the heap based upad a base address and an offset.
 */

STATIC INLINE Heap_Block *_Heap_Block_at(
  void       *base,
  unsigned32  offset
)
{
  return (Heap_Block *) _Addresses_Add_offset( (void *)base, offset );
}

/*PAGE
 *
 *  _Heap_User_block_at
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
STATIC INLINE Heap_Block *_Heap_User_block_at(
  void       *base
)
{
  unsigned32         offset;
 
  offset = *(((unsigned32 *) base) - 1);
  return _Heap_Block_at( base, -offset + -HEAP_BLOCK_USED_OVERHEAD);
}

/*PAGE
 *
 *  _Heap_Is_previous_block_free
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the previous block of the_block
 *  is free, and FALSE otherwise.
 */

STATIC INLINE boolean _Heap_Is_previous_block_free (
  Heap_Block *the_block
)
{
  return !(the_block->back_flag & HEAP_BLOCK_USED);
}

/*PAGE
 *
 *  _Heap_Is_block_free
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the block is free, and FALSE otherwise.
 */

STATIC INLINE boolean _Heap_Is_block_free (
  Heap_Block *the_block
)
{
  return !(the_block->front_flag & HEAP_BLOCK_USED);
}

/*PAGE
 *
 *  _Heap_Is_block_used
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the block is currently allocated,
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Heap_Is_block_used (
  Heap_Block *the_block
)
{
  return (the_block->front_flag & HEAP_BLOCK_USED);
}

/*PAGE
 *
 *  _Heap_Block_size
 *
 *  DESCRIPTION:
 *
 *  This function returns the size of the_block in bytes.
 */

STATIC INLINE unsigned32 _Heap_Block_size (
  Heap_Block *the_block
)
{
  return (the_block->front_flag & ~HEAP_BLOCK_USED);
}

/*PAGE
 *
 *  _Heap_Start_of_user_area
 *
 *  DESCRIPTION:
 *
 *  This function returns the starting address of the portion of the block
 *  which the user may access.
 */

STATIC INLINE void *_Heap_Start_of_user_area (
  Heap_Block *the_block
)
{
  return (void *) &the_block->next;
}

/*PAGE
 *
 *  _Heap_Is_block_in
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_block is within the memory area
 *  managed by the_heap, and FALSE otherwise.
 */

STATIC INLINE boolean _Heap_Is_block_in (
  Heap_Control *the_heap,
  Heap_Block   *the_block
)
{
  return _Addresses_Is_in_range( the_block, the_heap->start, the_heap->final );
}

/*PAGE
 *
 *  _Heap_Is_page_size_valid
 *
 *  DESCRIPTION:
 *
 *  This function validates a specified heap page size.  If the page size
 *  is 0 or if lies outside a page size alignment boundary it is invalid
 *  and FALSE is returned.  Otherwise, the page size is valid and TRUE is
 *  returned.
 */

STATIC INLINE boolean _Heap_Is_page_size_valid(
  unsigned32 page_size
)
{
  return ((page_size != 0) &&
         ((page_size % CPU_HEAP_ALIGNMENT) == 0));
}

/*PAGE
 *
 *  _Heap_Build_flag
 *
 *  DESCRIPTION:
 *
 *  This function returns the block flag composed of size and in_use_flag.
 *  The flag returned is suitable for use as a back or front flag in a
 *  heap block.
 */

STATIC INLINE unsigned32 _Heap_Build_flag (
  unsigned32 size,
  unsigned32 in_use_flag
)
{
  return  size | in_use_flag;
}

#endif
/* end of include file */
