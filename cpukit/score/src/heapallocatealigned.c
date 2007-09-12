/*
 *  Heap Handler
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

#if defined(RTEMS_HEAP_DEBUG)

static void
check_result(
  Heap_Control* the_heap,
  Heap_Block* the_block,
  _H_uptr_t user_addr,
  _H_uptr_t aligned_user_addr,
  uint32_t size)
{
  _H_uptr_t const user_area = _H_p2u(_Heap_User_area(the_block));
  _H_uptr_t const block_end = _H_p2u(the_block)
    + _Heap_Block_size(the_block) + HEAP_BLOCK_HEADER_OFFSET;
  _H_uptr_t const user_end = aligned_user_addr + size;
  _H_uptr_t const heap_start = _H_p2u(the_heap->start) + HEAP_OVERHEAD;
  _H_uptr_t const heap_end = _H_p2u(the_heap->final)
    + HEAP_BLOCK_HEADER_OFFSET;
  uint32_t const page_size = the_heap->page_size;

  _HAssert(user_addr == user_area);
  _HAssert(aligned_user_addr - user_area < page_size);
  _HAssert(aligned_user_addr >= user_area);
  _HAssert(aligned_user_addr < block_end);
  _HAssert(user_end > user_area);
  _HAssert(user_end <= block_end);
  _HAssert(aligned_user_addr >= heap_start);
  _HAssert(aligned_user_addr < heap_end);
  _HAssert(user_end > heap_start);
  _HAssert(user_end <= heap_end);
}

#else /* !defined(RTEMS_HEAP_DEBUG) */

#define check_result(a, b, c, d, e) ((void)0)

#endif /* !defined(RTEMS_HEAP_DEBUG) */

/*
 * Allocate block of size 'alloc_size' from 'the_block' belonging to
 * 'the_heap'. Split 'the_block' if possible, otherwise allocate it entirely.
 * When split, make the upper part used, and leave the lower part free.
 * Return the block allocated.
 *
 * NOTE: this is similar to _Heap_Block_allocate(), except it makes different
 * part of the split block used, and returns address of the block instead of its
 * size. We do need such variant for _Heap_Allocate_aligned() as we can't allow
 * user pointer to be too far from the beginning of the block, so that we can
 * recover start-of-block address from the user pointer without additional
 * information stored in the heap.
 */
static
Heap_Block *block_allocate(
  Heap_Control  *the_heap,
  Heap_Block    *the_block,
  uint32_t      alloc_size)
{
  Heap_Statistics *const stats = &the_heap->stats;
  uint32_t const block_size = _Heap_Block_size(the_block);
  uint32_t const the_rest = block_size - alloc_size;

  _HAssert(_Heap_Is_aligned(block_size, the_heap->page_size));
  _HAssert(_Heap_Is_aligned(alloc_size, the_heap->page_size));
  _HAssert(alloc_size <= block_size);
  _HAssert(_Heap_Is_prev_used(the_block));

  if(the_rest >= the_heap->min_block_size) {
    /* Split the block so that lower part is still free, and upper part
       becomes used. */
    the_block->size = the_rest | HEAP_PREV_USED;
    the_block = _Heap_Block_at(the_block, the_rest);
    the_block->prev_size = the_rest;
    the_block->size = alloc_size;
  }
  else {
    /* Don't split the block as remainder is either zero or too small to be
       used as a separate free block. Change 'alloc_size' to the size of the
       block and remove the block from the list of free blocks. */
    _Heap_Block_remove(the_block);
    alloc_size = block_size;
    stats->free_blocks -= 1;
  }
  /* Mark the block as used (in the next block). */
  _Heap_Block_at(the_block, alloc_size)->size |= HEAP_PREV_USED;
  /* Update statistics */
  stats->free_size -= alloc_size;
  if(stats->min_free_size > stats->free_size)
    stats->min_free_size = stats->free_size;
  stats->used_blocks += 1;
  return the_block;
}


/*PAGE
 *
 *  _Heap_Allocate_aligned
 *
 *  This kernel routine allocates the requested size of memory
 *  from the specified heap so that returned address is aligned according to
 *  the 'alignment'.
 *
 *  Input parameters:
 *    the_heap  - pointer to the heap control block.
 *    size      - size in bytes of the memory block to allocate.
 *    alignment - required user pointer alignment in bytes
 *
 *  Output parameters:
 *    returns - starting address of memory block allocated. The address is
 *              aligned on specified boundary.
 */

void *_Heap_Allocate_aligned(
  Heap_Control *the_heap,
  size_t        size,
  uint32_t      alignment
)
{
  uint32_t search_count;
  Heap_Block *the_block;

  void *user_ptr = NULL;
  uint32_t  const page_size = the_heap->page_size;
  Heap_Statistics *const stats = &the_heap->stats;
  Heap_Block *const tail = _Heap_Tail(the_heap);

  uint32_t const end_to_user_offs = size - HEAP_BLOCK_HEADER_OFFSET;

  uint32_t const the_size =
    _Heap_Calc_block_size(size, page_size, the_heap->min_block_size);

  if(the_size == 0)
    return NULL;

  if(alignment == 0)
    alignment = CPU_ALIGNMENT;

  /* Find large enough free block that satisfies the alignment requirements. */

  for(the_block = _Heap_First(the_heap), search_count = 0;
      the_block != tail;
      the_block = the_block->next, ++search_count)
  {
    uint32_t const block_size = _Heap_Block_size(the_block);

    /* As we always coalesce free blocks, prev block must have been used. */
    _HAssert(_Heap_Is_prev_used(the_block));

    if(block_size >= the_size) { /* the_block is large enough. */

      _H_uptr_t user_addr;
      _H_uptr_t aligned_user_addr;
      _H_uptr_t const user_area = _H_p2u(_Heap_User_area(the_block));

      /* Calculate 'aligned_user_addr' that will become the user pointer we
         return. It should be at least 'end_to_user_offs' bytes less than the
         the 'block_end' and should be aligned on 'alignment' boundary.
         Calculations are from the 'block_end' as we are going to split free
         block so that the upper part of the block becomes used block. */
      _H_uptr_t const block_end = _H_p2u(the_block) + block_size;
      aligned_user_addr = block_end - end_to_user_offs;
      _Heap_Align_down_uptr(&aligned_user_addr, alignment);

      /* 'user_addr' is the 'aligned_user_addr' further aligned down to the
         'page_size' boundary. We need it as blocks' user areas should begin
         only at 'page_size' aligned addresses */
      user_addr = aligned_user_addr;
      _Heap_Align_down_uptr(&user_addr, page_size);

      /* Make sure 'user_addr' calculated didn't run out of 'the_block'. */
      if(user_addr >= user_area) {

        /* The block seems to be acceptable. Check if the remainder of
           'the_block' is less than 'min_block_size' so that 'the_block' won't
           actually be split at the address we assume. */
        if(user_addr - user_area < the_heap->min_block_size) {

          /* The block won't be split, so 'user_addr' will be equal to the
             'user_area'. */
          user_addr = user_area;

          /* We can't allow the distance between 'user_addr' and
           'aligned_user_addr' to be outside of [0,page_size) range. If we do,
           we will need to store this distance somewhere to be able to
           resurrect the block address from the user pointer. (Having the
           distance within [0,page_size) range allows resurrection by
           aligning user pointer down to the nearest 'page_size' boundary.) */
          if(aligned_user_addr - user_addr >= page_size) {

            /* The user pointer will be too far from 'user_addr'. See if we
               can make 'aligned_user_addr' to be close enough to the
               'user_addr'. */
            aligned_user_addr = user_addr;
            _Heap_Align_up_uptr(&aligned_user_addr, alignment);
            if(aligned_user_addr - user_addr >= page_size) {
              /* No, we can't use the block */
              aligned_user_addr = 0;
            }
          }
        }

        if(aligned_user_addr) {

          /* The block is indeed acceptable: calculate the size of the block
             to be allocated and perform allocation. */
          uint32_t const alloc_size =
            block_end - user_addr + HEAP_BLOCK_USER_OFFSET;

          _HAssert(_Heap_Is_aligned_ptr((void*)aligned_user_addr, alignment));

          the_block = block_allocate(the_heap, the_block, alloc_size);

          stats->searches += search_count + 1;
          stats->allocs += 1;

          check_result(the_heap, the_block, user_addr,
            aligned_user_addr, size);

          user_ptr = (void*)aligned_user_addr;
          break;
        }
      }
    }
  }

  if(stats->max_search < search_count)
    stats->max_search = search_count;

  return user_ptr;
}
