/*  heap.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the heap handler.
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

#ifndef _RTEMS_SCORE_HEAP_INL
#define _RTEMS_SCORE_HEAP_INL

/*
 * WARNING: this file is only visually checked against
 * '../../../inline/rtems/score/heap.inl'. Use those file for reference
 * if you encounter problems.
 */

#include <rtems/score/address.h>

/*PAGE
 *
 *  _Heap_Head
 */

#define _Heap_Head( _the_heap ) (&(_the_heap)->free_list)

/*PAGE
 *
 *  _Heap_Tail
 */

#define _Heap_Tail( _the_heap ) (&(_the_heap)->free_list)

/*PAGE
 *
 *  _Heap_First
 */

#define _Heap_First( _the_heap ) (_Heap_Head(_the_heap)->next)

/*PAGE
 *
 *  _Heap_Last
 */

#define _Heap_Last( _the_heap )  (_Heap_Tail(_the_heap)->prev)

/*PAGE
 *
 *  _Heap_Block_remove
 *
 */

#define _Heap_Block_remove( _the_block ) \
  do { \
    Heap_Block *block = (_the_block); \
    Heap_Block *next = block->next; \
    Heap_Block *prev = block->prev; \
    prev->next = next; \
    next->prev = prev; \
  } while(0)

/*PAGE
 *
 *  _Heap_Block_replace
 *
 */

#define _Heap_Block_replace( _old_block,  _new_block ) \
  do { \
    Heap_Block *block = (_old_block); \
    Heap_Block *next = block->next; \
    Heap_Block *prev = block->prev; \
    block = (_new_block); \
    block->next = next; \
    block->prev = prev; \
    next->prev = prev->next = block; \
  } while(0)

/*PAGE
 *
 *  _Heap_Block_insert_after
 *
 */

#define _Heap_Block_insert_after(  _prev_block, _the_block ) \
  do { \
    Heap_Block *prev = (_prev_block); \
    Heap_Block *block = (_the_block); \
    Heap_Block *next = prev->next; \
    block->next  = next; \
    block->prev  = prev; \
    next->prev = prev->next = block; \
  } while(0)

/*PAGE
 *
 *  _Heap_Is_aligned
 */

#define _Heap_Is_aligned( _value, _alignment ) \
  (((_value) % (_alignment)) == 0)

/*PAGE
 *
 *  _Heap_Align_up
 */

#define _Heap_Align_up( _value, _alignment ) \
  do { \
    uint32_t v = *(_value); \
    uint32_t a = (_alignment); \
    uint32_t r = v % a; \
    *(_value) = r ? v - r + a : v; \
  } while(0)

/*PAGE
 *
 *  _Heap_Align_down
 */

#define _Heap_Align_down( _value, _alignment ) \
  do { \
    uint32_t v = *(_value); \
    *(_value) = v - (v % (_alignment)); \
  } while(0)

/*PAGE
 *
 *  _Heap_Is_aligned_ptr
 */

#define _Heap_Is_aligned_ptr( _ptr, _alignment ) \
  ((_H_p2u(_ptr) % (_alignment)) == 0)

/*PAGE
 *
 *  _Heap_Align_up_uptr
 */

#define _Heap_Align_up_uptr( _value, _alignment ) \
  do { \
    _H_uptr_t v = *(_value); \
    uint32_t a = (_alignment); \
    _H_uptr_t r = v % a; \
    *(_value) = r ? v - r + a : v; \
  } while(0)

/*PAGE
 *
 *  _Heap_Align_down_uptr
 */

#define _Heap_Align_down_uptr( _value, _alignment ) \
  do { \
    _H_uptr_t v = *(_value); \
    *(_value) = v - (v % (_alignment)); \
  } while(0)

/*PAGE
 *
 *  _Heap_Block_at
 */

#define _Heap_Block_at( _base, _offset ) \
  ( (Heap_Block *) _Addresses_Add_offset( (_base), (_offset) ) )

/*PAGE
 *
 *  _Heap_User_area
 */

#define _Heap_User_area( _the_block ) \
  ((void *) _Addresses_Add_offset( (_the_block), HEAP_BLOCK_USER_OFFSET ))

/*PAGE
 *
 *  _Heap_Start_of_block
 */
 
#define _Heap_Start_of_block( _the_heap, _base, _the_block_ptr ) \
  do { \
    _H_uptr_t addr = _H_p2u(_base); \
    _Heap_Align_down( &addr, (_the_heap)->page_size ); \
    *(_the_block_ptr) = (Heap_Block *)(addr - HEAP_BLOCK_USER_OFFSET); \
  } while(0)

/*PAGE
 *
 *  _Heap_Is_prev_used
 */

#define _Heap_Is_prev_used( _the_block ) \
  ((_the_block)->size & HEAP_PREV_USED)

/*PAGE
 *
 *  _Heap_Block_size
 */

#define _Heap_Block_size( _the_block )    \
  ((_the_block)->size & ~HEAP_PREV_USED)

/*PAGE
 *
 *  _Heap_Is_block_in
 */

#define _Heap_Is_block_in( _the_heap, _the_block ) \
  ( _Addresses_Is_in_range( (_the_block), \
    (_the_heap)->start, (_the_heap)->final ) )

#endif
/* end of include file */
