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


#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

/*PAGE
 *
 *  _Heap_Walk
 *
 *  This kernel routine walks the heap and verifies its correctness.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header
 *    source    - a numeric indicator of the invoker of this routine
 *    do_dump   - when TRUE print the information
 *
 *  Output parameters: NONE
 */

#ifndef RTEMS_DEBUG

void _Heap_Walk(
  Heap_Control  *the_heap,
  int            source,
  boolean        do_dump
)
{
}

#else

#include <stdio.h>
#include <unistd.h>

void _Heap_Walk(
  Heap_Control  *the_heap,
  int            source,
  boolean        do_dump
)
{
  Heap_Block *the_block  = 0;  /* avoid warnings */
  Heap_Block *next_block = 0;  /* avoid warnings */
  int         notdone = 1;
  int         error = 0;
  int         passes = 0;

  /*
   * We don't want to allow walking the heap until we have
   * transferred control to the user task so we watch the
   * system state.
   */

  if ( !_System_state_Is_up( _System_state_Get() ) )
    return;

  the_block = the_heap->start;

  if (do_dump == TRUE) {
    printf("\nPASS: %d  start @ 0x%p   final 0x%p,   first 0x%p  last 0x%p\n",
            source, the_heap->start, the_heap->final,
                  the_heap->first, the_heap->last
          );
  }

  /*
   * Handle the 1st block
   */

  if (the_block->back_flag != HEAP_DUMMY_FLAG) {
    printf("PASS: %d  Back flag of 1st block isn't HEAP_DUMMY_FLAG\n", source);
    error = 1;
  }

  while (notdone) {
    passes++;
    if (error && (passes > 10))
        abort();
    
    if (do_dump == TRUE) {
      printf("PASS: %d  Block @ 0x%p   Back %d,   Front %d",
              source, the_block,
              the_block->back_flag, the_block->front_flag);
      if ( _Heap_Is_block_free(the_block) ) {
        printf( "      Prev 0x%p,   Next 0x%p\n",
                          the_block->previous, the_block->next);
      } else {
        printf("\n");
      }
    }

    /*
     * Handle the last block
     */

    if ( the_block->front_flag != HEAP_DUMMY_FLAG ) {
      next_block = _Heap_Next_block(the_block);
      if ( the_block->front_flag != next_block->back_flag ) {
        error = 1;
        printf("PASS: %d  Front and back flags don't match\n", source);
        printf("         Current Block (%p):  Back - %d,  Front - %d",
               the_block, the_block->back_flag, the_block->front_flag);
        if (do_dump == TRUE) {
          if (_Heap_Is_block_free(the_block)) {
            printf("      Prev 0x%p,   Next 0x%p\n",
                   the_block->previous, the_block->next);
          } else {
            printf("\n");
          }
        } else {
          printf("\n");
        }
        printf("         Next Block (%p):     Back - %d,  Front - %d",
               next_block, next_block->back_flag, next_block->front_flag);
        if (do_dump == TRUE) {
          if (_Heap_Is_block_free(next_block)) {
            printf("      Prev 0x%p,   Next 0x%p\n",
                   the_block->previous, the_block->next);
          } else {
            printf("\n");
          }
        } else {
          printf("\n");
        }
      }
    }

    if (the_block->front_flag == HEAP_DUMMY_FLAG)
      notdone = 0;
    else
      the_block = next_block;
  }

  if (error)
      abort();
}
#endif
