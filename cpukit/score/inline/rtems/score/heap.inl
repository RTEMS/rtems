/** 
 *  @file  rtems/score/heap.inl
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

#ifndef _RTEMS_SCORE_HEAP_INL
#define _RTEMS_SCORE_HEAP_INL

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
  return &the_heap->free_list;
}

/**
 *  This function returns the tail of the specified heap.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Tail (
  Heap_Control *the_heap
)
{
  return &the_heap->free_list;
}

/**
 *  Return the first free block of the specified heap.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_First (
  Heap_Control *the_heap
)
{
  return _Heap_Head(the_heap)->next;
}

/*PAGE
 *
 *  _Heap_Last
 *
 *  DESCRIPTION:
 *
 *  Return the last free block of the specified heap.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Last (
  Heap_Control *the_heap
)
{
  return _Heap_Tail(the_heap)->prev;
}

/*PAGE
 *
 *  _Heap_Block_remove
 *
 *  DESCRIPTION:
 *
 *  This function removes 'the_block' from doubly-linked list.
 */

RTEMS_INLINE_ROUTINE void _Heap_Block_remove (
  Heap_Block *the_block
)
{
  Heap_Block *block = the_block;

  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;
  prev->next = next;
  next->prev = prev;
}

/**
 *  This function replaces @a old_block by @a new_block in doubly-linked list.
 */

RTEMS_INLINE_ROUTINE void _Heap_Block_replace (
  Heap_Block *old_block,
  Heap_Block *new_block
)
{
  Heap_Block *block = old_block;
  Heap_Block *next = block->next;
  Heap_Block *prev = block->prev;

  block = new_block;
  block->next = next;
  block->prev = prev;
  next->prev = prev->next = block;
}

/**
 *  This function inserts @a the_block after @a prev_block
 *  in doubly-linked list.
 */
RTEMS_INLINE_ROUTINE void _Heap_Block_insert_after (
  Heap_Block *prev_block,
  Heap_Block *the_block
)
{
  Heap_Block *prev = prev_block;
  Heap_Block *block = the_block;

  Heap_Block *next = prev->next;
  block->next  = next;
  block->prev  = prev;
  next->prev = prev->next = block;
}

/**
 *  Return TRUE if @a value is a multiple of @a alignment,  FALSE otherwise
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_aligned (
  uint32_t  value,
  uint32_t  alignment
)
{
  return (value % alignment) == 0;
}

/**
 *  Align @a *value up to the nearest multiple of @a alignment.
 */

RTEMS_INLINE_ROUTINE void _Heap_Align_up (
  uint32_t *value,
  uint32_t  alignment
)
{
  uint32_t v = *value;
  uint32_t a = alignment;
  uint32_t r = v % a;
  *value = r ? v - r + a : v;
}

/**
 *  Align @a *value down to the nearest multiple of @a alignment.
 */

RTEMS_INLINE_ROUTINE void _Heap_Align_down (
  uint32_t *value,
  uint32_t  alignment
)
{
  uint32_t v = *value;
  *value = v - (v % alignment);
}

/**
 *  Return TRUE if @a ptr is aligned at @a alignment boundary,
 *  FALSE otherwise
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_aligned_ptr (
  void *ptr,
  uint32_t  alignment
)
{
  return (_H_p2u(ptr) % alignment) == 0;
}

/**
 *  Align @a *value up to the nearest multiple of @a alignment.
 */

RTEMS_INLINE_ROUTINE void _Heap_Align_up_uptr (
  _H_uptr_t *value,
  uint32_t  alignment
)
{
  _H_uptr_t v = *value;
  uint32_t a = alignment;
  _H_uptr_t r = v % a;
  *value = r ? v - r + a : v;
}

/**
 *  Align @a *value down to the nearest multiple of @a alignment.
 */

RTEMS_INLINE_ROUTINE void _Heap_Align_down_uptr (
  _H_uptr_t *value,
  uint32_t  alignment
)
{
  _H_uptr_t v = *value;
  *value = v - (v % alignment);
}

/**
 *  This function calculates and returns a block's location (address)
 *  in the heap based upon a base address @a base and an @a offset.
 */

RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  void       *base,
  uint32_t  offset
)
{
  return (Heap_Block *) _Addresses_Add_offset( base, offset );
}

/**
 *  This function returns the starting address of the portion of @a the_block
 *  which the user may access.
 */

RTEMS_INLINE_ROUTINE void *_Heap_User_area (
  Heap_Block *the_block
)
{
  return (void *) _Addresses_Add_offset ( the_block, HEAP_BLOCK_USER_OFFSET );
}

/**
 *  Fill @a *the_block with the address of the beginning of the block given
 *  pointer to the user accessible area @a base.
 */

RTEMS_INLINE_ROUTINE void _Heap_Start_of_block (
  Heap_Control  *the_heap,
  void          *base,
  Heap_Block   **the_block
)
{
  _H_uptr_t addr = _H_p2u(base);
  /* The address passed could be greater than the block address plus
   * HEAP_BLOCK_USER_OFFSET as _Heap_Allocate_aligned() may produce such user
   * pointers. To get rid of this offset we need to align the address down
   * to the nearest 'page_size' boundary. */
  _Heap_Align_down_uptr ( &addr, the_heap->page_size );
  *the_block = (Heap_Block *)(addr - HEAP_BLOCK_USER_OFFSET);
}

/**
 *  This function returns TRUE if the previous block of @a the_block
 *  is in use, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_prev_used (
  Heap_Block *the_block
)
{
  return (the_block->size & HEAP_PREV_USED);
}

/**
 *  This function returns the size of @a the_block in bytes.
 */

RTEMS_INLINE_ROUTINE uint32_t _Heap_Block_size (
  Heap_Block *the_block
)
{
  return (the_block->size & ~HEAP_PREV_USED);
}

/**
 *  This function returns TRUE if @a the_block is within the memory area
 *  managed by @a the_heap, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Heap_Is_block_in (
  Heap_Control *the_heap,
  Heap_Block   *the_block
)
{
  return _Addresses_Is_in_range( the_block, the_heap->start, the_heap->final );
}

/**@}*/

#endif
/* end of include file */
