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

#ifndef __HEAP_inl
#define __HEAP_inl

#include <rtems/score/address.h>

/*PAGE
 *
 *  _Heap_Head
 */

#define _Heap_Head( _the_heap ) \
  ((Heap_Block *)&(_the_heap)->start)

/*PAGE
 *
 *  _Heap_Tail
 */

#define _Heap_Tail( _the_heap ) \
  ((Heap_Block *)&(_the_heap)->final)

/*PAGE
 *
 *  _Heap_Previous_block
 */

#define _Heap_Previous_block( _the_block ) \
  ( (Heap_Block *) _Addresses_Subtract_offset( \
      (void *)(_the_block), \
      (_the_block)->back_flag & ~ HEAP_BLOCK_USED \
    ) \
  )

/*PAGE
 *
 *  _Heap_Next_block
 */

#define _Heap_Next_block( _the_block ) \
  ( (Heap_Block *) _Addresses_Add_offset( \
      (void *)(_the_block), \
      (_the_block)->front_flag & ~ HEAP_BLOCK_USED \
    ) \
  )

/*PAGE
 *
 *  _Heap_Block_at
 */

#define _Heap_Block_at( _base, _offset ) \
  ( (Heap_Block *) \
     _Addresses_Add_offset( (void *)(_base), (_offset) ) )

/*PAGE
 *
 *  _Heap_User_block_at
 *
 */
 
#define _Heap_User_block_at( _base ) \
  _Heap_Block_at( \
    (_base), \
    -*(((unsigned32 *) (_base)) - 1) + -HEAP_BLOCK_USED_OVERHEAD \
  )

/*PAGE
 *
 *  _Heap_Is_previous_block_free
 */

#define _Heap_Is_previous_block_free( _the_block ) \
  ( !((_the_block)->back_flag & HEAP_BLOCK_USED) )

/*PAGE
 *
 *  _Heap_Is_block_free
 */

#define _Heap_Is_block_free( _the_block ) \
  ( !((_the_block)->front_flag & HEAP_BLOCK_USED) )

/*PAGE
 *
 *  _Heap_Is_block_used
 */

#define _Heap_Is_block_used( _the_block ) \
  ((_the_block)->front_flag & HEAP_BLOCK_USED)

/*PAGE
 *
 *  _Heap_Block_size
 */

#define _Heap_Block_size( _the_block )    \
  ((_the_block)->front_flag & ~HEAP_BLOCK_USED)

/*PAGE
 *
 *  _Heap_Start_of_user_area
 */

#define _Heap_Start_of_user_area( _the_block ) \
  ((void *) &(_the_block)->next)

/*PAGE
 *
 *  _Heap_Is_block_in
 */

#define _Heap_Is_block_in( _the_heap, _the_block ) \
  ( ((_the_block) >= (_the_heap)->start) && \
    ((_the_block) <= (_the_heap)->final) )

/*PAGE
 *
 *  _Heap_Is_page_size_valid
 */

#define _Heap_Is_page_size_valid( _page_size ) \
  ( ((_page_size) != 0) && \
    (((_page_size) % CPU_HEAP_ALIGNMENT) == 0) )

/*PAGE
 *
 *  _Heap_Build_flag
 */

#define _Heap_Build_flag( _size, _in_use_flag ) \
  ( (_size) | (_in_use_flag))

#endif
/* end of include file */
