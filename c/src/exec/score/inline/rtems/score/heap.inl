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

#include <rtems/core/address.h>

/*PAGE
 *
 *  _Heap_Head
 *
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
 *  _Heap_User_Block_at
 *
 */
 
STATIC INLINE Heap_Block *_Heap_User_Block_at(
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
