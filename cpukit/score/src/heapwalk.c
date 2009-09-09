/**
 * @file
 *
 * @ingroup ScoreHeap
 *
 * @brief Heap Handler implementation.
 */

/*
 *  COPYRIGHT ( c ) 1989-2007.
 *  On-Line Applications Research Corporation ( OAR ).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>
#include <rtems/score/interr.h>
#include <rtems/bspIo.h>

static void _Heap_Walk_printk( int source, bool dump, bool error, const char *fmt, ... )
{
  if ( dump ) {
    va_list ap;

    if ( error ) {
      printk( "FAIL[%d]: ", source );
    } else {
      printk( "PASS[%d]: ", source );
    }

    va_start( ap, fmt );
    vprintk( fmt, ap );
    va_end( ap );
  }
}

static bool _Heap_Walk_check_free_list(
  int source,
  bool dump,
  Heap_Control *heap
)
{
  uintptr_t const page_size = heap->page_size;
  const Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  const Heap_Block *const first_free_block = _Heap_Free_list_first( heap );
  const Heap_Block *prev_block = free_list_tail;
  const Heap_Block *free_block = first_free_block;

  while ( free_block != free_list_tail ) {
    if ( !_Heap_Is_block_in_heap( heap, free_block ) ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "free block 0x%08x: not in heap\n",
        free_block
      );

      return false;
    }

    if (
      !_Heap_Is_aligned( _Heap_Alloc_area_of_block( free_block ), page_size )
    ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "free block 0x%08x: alloc area not page aligned\n",
        free_block
      );

      return false;
    }

    if ( _Heap_Is_used( free_block ) ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "free block 0x%08x: is used\n",
        free_block
      );

      return false;
    }

    if ( free_block->prev != prev_block ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "free block 0x%08x: invalid previous block 0x%08x\n",
        free_block,
        free_block->prev
      );

      return false;
    }

    prev_block = free_block;
    free_block = free_block->next;
  }

  return true;
}

static bool _Heap_Walk_is_in_free_list(
  Heap_Control *heap,
  Heap_Block *block
)
{
  const Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  const Heap_Block *free_block = _Heap_Free_list_first( heap );

  while ( free_block != free_list_tail ) {
    if ( free_block == block ) {
      return true;
    }
    free_block = free_block->next;
  }

  return false;
}

static bool _Heap_Walk_check_control(
  int source,
  bool dump,
  Heap_Control *heap
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const min_block_size = heap->min_block_size;
  Heap_Block *const first_free_block = _Heap_Free_list_first( heap );
  Heap_Block *const last_free_block = _Heap_Free_list_last( heap );
  Heap_Block *const first_block = heap->first_block;
  Heap_Block *const last_block = heap->last_block;

  _Heap_Walk_printk(
    source,
    dump,
    false,
    "page size %u, min block size %u\n"
      "\tarea begin 0x%08x, area end 0x%08x\n"
      "\tfirst block 0x%08x, last block 0x%08x\n"
      "\tfirst free 0x%08x, last free 0x%08x\n",
    page_size, min_block_size,
    heap->area_begin, heap->area_end,
    first_block, last_block,
    first_free_block, last_free_block
  );

  if ( page_size == 0 ) {
    _Heap_Walk_printk( source, dump, true, "page size is zero\n" );

    return false;
  }

  if ( !_Addresses_Is_aligned( (void *) page_size ) ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "page size %u not CPU aligned\n",
      page_size
    );

    return false;
  }

  if ( !_Heap_Is_aligned( min_block_size, page_size ) ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "min block size %u not page aligned\n",
      min_block_size
    );

    return false;
  }

  if (
    !_Heap_Is_aligned( _Heap_Alloc_area_of_block( first_block ), page_size )
  ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "first block 0x%08x: alloc area not page aligned\n",
      first_block
    );

    return false;
  }

  if ( !_Heap_Is_prev_used( first_block ) ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "first block: HEAP_PREV_BLOCK_USED is cleared\n"
    );

    return false;
  }

  if ( first_block->prev_size != page_size ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "first block: prev size %u != page size %u\n",
      first_block->prev_size,
      page_size
    );

    return false;
  }

  if ( _Heap_Is_free( last_block ) ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "last block: is free\n"
    );

    return false;
  }

  return _Heap_Walk_check_free_list( source, dump, heap );
}

static bool _Heap_Walk_check_free_block(
  int source,
  bool dump,
  Heap_Control *heap,
  Heap_Block *block
)
{
  Heap_Block *const free_list_tail = _Heap_Free_list_tail( heap );
  Heap_Block *const free_list_head = _Heap_Free_list_head( heap );
  Heap_Block *const first_free_block = _Heap_Free_list_first( heap );
  Heap_Block *const last_free_block = _Heap_Free_list_last( heap );
  bool const prev_used = _Heap_Is_prev_used( block );
  uintptr_t const block_size = _Heap_Block_size( block );
  Heap_Block *const next_block = _Heap_Block_at( block, block_size );

  _Heap_Walk_printk(
    source,
    dump,
    false,
    "block 0x%08x: prev 0x%08x%s, next 0x%08x%s\n",
    block,
    block->prev,
    block->prev == first_free_block ?
      " (= first)"
        : (block->prev == free_list_head ? " (= head)" : ""),
    block->next,
    block->next == last_free_block ?
      " (= last)"
        : (block->next == free_list_tail ? " (= tail)" : "")
  );

  if ( block_size != next_block->prev_size ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "block 0x%08x: size %u != size %u (in next block 0x%08x)\n",
      block,
      block_size,
      next_block->prev_size,
      next_block
    );

    return false;
  }

  if ( !prev_used ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "block 0x%08x: two consecutive blocks are free\n",
      block
    );

    return false;
  }

  if ( !_Heap_Walk_is_in_free_list( heap, block ) ) {
    _Heap_Walk_printk(
      source,
      dump,
      true,
      "block 0x%08x: free block not in free list\n",
      block
    );

    return false;
  }

  return true;
}

bool _Heap_Walk(
  Heap_Control *heap,
  int source,
  bool dump
)
{
  uintptr_t const page_size = heap->page_size;
  uintptr_t const min_block_size = heap->min_block_size;
  Heap_Block *const last_block = heap->last_block;
  Heap_Block *block = heap->first_block;

  if ( !_System_state_Is_up( _System_state_Get() ) ) {
    return true;
  }

  if ( !_Heap_Walk_check_control( source, dump, heap ) ) {
    return false;
  }

  while ( block != last_block ) {
    uintptr_t const block_begin = (uintptr_t) block;
    uintptr_t const block_size = _Heap_Block_size( block );
    bool const prev_used = _Heap_Is_prev_used( block );
    Heap_Block *const next_block = _Heap_Block_at( block, block_size );
    uintptr_t const next_block_begin = (uintptr_t) next_block;

    if ( prev_used ) {
      _Heap_Walk_printk(
        source,
        dump,
        false,
        "block 0x%08x: size %u\n",
        block,
        block_size
      );
    } else {
      _Heap_Walk_printk(
        source,
        dump,
        false,
        "block 0x%08x: size %u, prev_size %u\n",
        block,
        block_size,
        block->prev_size
      );
    }

    if ( !_Heap_Is_block_in_heap( heap, next_block ) ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "block 0x%08x: next block 0x%08x not in heap\n",
        block,
        next_block
      );
      
      return false;
    }

    if ( !_Heap_Is_aligned( block_size, page_size ) ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "block 0x%08x: block size %u not page aligned\n",
        block,
        block_size
      );
      
      return false;
    }

    if ( block_size < min_block_size ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "block 0x%08x: size %u < min block size %u\n",
        block,
        block_size,
        min_block_size
      );
      
      return false;
    }

    if ( next_block_begin <= block_begin ) {
      _Heap_Walk_printk(
        source,
        dump,
        true,
        "block 0x%08x: next block 0x%08x is not a successor\n",
        block,
        next_block
      );
      
      return false;
    }
   
    if ( !_Heap_Is_prev_used( next_block ) ) {
      if ( !_Heap_Walk_check_free_block( source, dump, heap, block ) ) {
        return false;
      }
    }

    block = next_block;
  }

  return true;
}
