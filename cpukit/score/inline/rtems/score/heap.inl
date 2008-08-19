/** 
 *  @file  rtems/score/heap.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the heap handler.
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
# error "Never use <rtems/score/heap.inl> directly; include <rtems/score/heap.h> instead."
#endif

#ifndef _RTEMS_SCORE_HEAP_INL
#define _RTEMS_SCORE_HEAP_INL

/**
 *  @addtogroup ScoreHeap 
 *  @{
 */

#include <rtems/score/address.h>

/**
 *  This function returns the head of the specified heap.
 *
 *  @param[in] the_heap points to the heap being operated upon
 *
 *  @return This method returns a pointer to the dummy head of the free
 *          block list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Head (
  Heap_Control *the_heap
)
{
  return &the_heap->free_list;
}

/**
 *  This function returns the tail of the specified heap.
 *
 *  @param[in] the_heap points to the heap being operated upon
 *
 *  @return This method returns a pointer to the dummy tail of the heap
 *          free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Tail (
  Heap_Control *the_heap
)
{
  return &the_heap->free_list;
}

/**
 *  Return the first free block of the specified heap.
 *
 *  @param[in] the_heap points to the heap being operated upon
 *
 *  @return This method returns a pointer to the first free block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_First (
  Heap_Control *the_heap
)
{
  return _Heap_Head(the_heap)->next;
}

/**
 *  Return the last free block of the specified heap.
 *
 *  @param[in] the_heap points to the heap being operated upon
 *
 *  @return This method returns a pointer to the last block on the free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Last (
  Heap_Control *the_heap
)
{
  return _Heap_Tail(the_heap)->prev;
}

/**
 *  This function removes 'the_block' from doubly-linked list.
 *
 *  @param[in] the_block is the block to remove from the heap used block
 *             list.
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
 *  When a block is allocated, the memory is allocated from the low memory 
 *  address range of the block.  This means that the upper address range of 
 *  the memory block must be added to the free block list in place of the
 *  lower address portion being allocated.  This method is also used as part
 *  of resizing a block.
 *
 *  @param[in] old_block is the block which is currently on the list.
 *  @param[in] new_block is the new block which will replace it on the list.
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
 *  in the doubly-linked free block list.
 *
 *  @param[in] prev_block is the previous block in the free list.
 *  @param[in] the_block is the block being freed.
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
 *
 *  @param[in] value is the address to verify alignment of.
 *  @param[in] alignment is the alignment factor to verify.
 *
 *  @return This method returns TRUE if the address is aligned and false
 *          otherwise.
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
 *
 *  @param[in] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 *
 *  @return Upon return, @a value will contain the aligned result.
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
 *
 *  @param[in] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 *
 *  @return Upon return, @a value will contain the aligned result.
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
 *  FALSE otherwise.
 *
 *  @param[in] ptr is the pointer to verify alignment of.
 *  @param[in] alignment is the alignment factor.
 *
 *  @return This method returns TRUE if @a ptr is aligned at @a alignment
 *          boundary, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_aligned_ptr (
  void      *ptr,
  uint32_t  alignment
)
{
  return (_H_p2u(ptr) % alignment) == 0;
}

/**
 *  Align @a *value up to the nearest multiple of @a alignment.
 *
 *  @param[in] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 *
 *  @return Upon return, @a value will contain the aligned result.
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
 *
 *  @param[in] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 *
 *  @return Upon return, @a value will contain the aligned result.
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
 *
 *  @param[in] base is the base address of the memory area.
 *  @param[in] offset is the byte offset into @a base.
 *
 *  @return This method returns a pointer to the block's address.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
  void     *base,
  uint32_t  offset
)
{
  return (Heap_Block *) _Addresses_Add_offset( base, offset );
}

/**
 *  This function returns the starting address of the portion of @a the_block
 *  which the user may access.
 *
 *  @param[in] the_block is the heap block to find the user area of.
 *
 *  @return This method returns a pointer to the start of the user area in
 *          the block.
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
 *
 *  @param[in] the_heap points to the heap being operated upon
 *  @param[in] base points to the user area in the block.
 *  @param[in] the_block will be the address of the heap block.
 *
 *  @return This method returns a pointer to the heap block based upon the
 *               given heap and user pointer.
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
 *
 *  @param[in] the_block is the block to operate upon.
 *
 *  @return This method returns TRUE if the previous block is used and FALSE
 *          if the previous block is free.
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_prev_used (
  Heap_Block *the_block
)
{
  return (the_block->size & HEAP_PREV_USED);
}

/**
 *  This function returns the size of @a the_block in bytes.
 *
 *  @param[in] the_block is the block to operate upon.
 *
 *  @return This method returns the size of the specified heap block in bytes.
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
 *
 *  @param[in] the_heap points to the heap being operated upon
 *  @param[in] the_block is the block address to check.
 *
 *  @return This method returns TRUE if @a the_block appears to have been
 *          allocated from @a the_heap, and FALSE otherwise.
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
