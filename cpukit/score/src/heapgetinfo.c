/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */


#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

/*PAGE
 *
 *  _Heap_Get_information
 *
 *  This kernel routine walks the heap and tots up the free and allocated
 *  sizes.  Derived from _Heap_Walk.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header
 *    the_info  - pointer for information to be returned
 *
 *  Output parameters: 
 *    *the_info  - contains information about heap
 *    return 0=success, otherwise heap is corrupt.
 */


Heap_Get_information_status _Heap_Get_information(
  Heap_Control            *the_heap,
  Heap_Information_block  *the_info
)
{
  Heap_Block *the_block  = 0;  /* avoid warnings */
  Heap_Block *next_block = 0;  /* avoid warnings */
  int         notdone = 1;


  the_info->free_blocks = 0;
  the_info->free_size   = 0;
  the_info->used_blocks = 0;
  the_info->used_size   = 0;

  /*
   * We don't want to allow walking the heap until we have
   * transferred control to the user task so we watch the
   * system state.
   */

  if ( !_System_state_Is_up( _System_state_Get() ) )
    return HEAP_GET_INFORMATION_SYSTEM_STATE_ERROR;

  the_block = the_heap->start;

  /*
   * Handle the 1st block
   */

  if ( the_block->back_flag != HEAP_DUMMY_FLAG ) {
    return HEAP_GET_INFORMATION_BLOCK_ERROR;
  }

  while (notdone) {
      
      /*
       * Accumulate size
       */

      if ( _Heap_Is_block_free(the_block) ) {
        the_info->free_blocks++;
        the_info->free_size += _Heap_Block_size(the_block);
      } else {
        the_info->used_blocks++;
        the_info->used_size += _Heap_Block_size(the_block);
      }
    
      /*
       * Handle the last block
       */

      if ( the_block->front_flag != HEAP_DUMMY_FLAG ) {
        next_block = _Heap_Next_block(the_block);
        if ( the_block->front_flag != next_block->back_flag ) {
          return HEAP_GET_INFORMATION_BLOCK_ERROR;
        }
      }

      if ( the_block->front_flag == HEAP_DUMMY_FLAG )
        notdone = 0;
      else
        the_block = next_block;
  }  /* while(notdone) */

  return HEAP_GET_INFORMATION_SUCCESSFUL;
}
