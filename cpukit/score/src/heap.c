/*
 *  Heap Handler
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


#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>

/*PAGE
 *
 *  _Heap_Initialize
 *
 *  This kernel routine initializes a heap.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of heap
 *    size             - size of heap
 *    page_size        - allocatable unit of memory
 *
 *  Output parameters:
 *    returns - maximum memory available if RTEMS_SUCCESSFUL
 *    0       - otherwise
 *
 *    This is what a heap looks like in memory immediately
 *    after initialization:
 *
 *            +--------------------------------+
 *     0      |  size = 0      | status = used |  a.k.a.  dummy back flag
 *            +--------------------------------+
 *     4      |  size = size-8 | status = free |  a.k.a.  front flag
 *            +--------------------------------+
 *     8      |  next     = PERM HEAP_TAIL     |
 *            +--------------------------------+
 *    12      |  previous = PERM HEAP_HEAD     |
 *            +--------------------------------+
 *            |                                |
 *            |      memory available          |
 *            |       for allocation           |
 *            |                                |
 *            +--------------------------------+
 *  size - 8  |  size = size-8 | status = free |  a.k.a.  back flag
 *            +--------------------------------+
 *  size - 4  |  size = 0      | status = used |  a.k.a.  dummy front flag
 *            +--------------------------------+
 */

unsigned32 _Heap_Initialize(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32           size,
  unsigned32           page_size
)
{
  Heap_Block        *the_block;
  unsigned32         the_size;

  if ( !_Heap_Is_page_size_valid( page_size ) ||
       (size < HEAP_MINIMUM_SIZE) )
    return 0;

  the_heap->page_size = page_size;
  the_size = size - HEAP_OVERHEAD;

  the_block             = (Heap_Block *) starting_address;
  the_block->back_flag  = HEAP_DUMMY_FLAG;
  the_block->front_flag = the_size;
  the_block->next       = _Heap_Tail( the_heap );
  the_block->previous   = _Heap_Head( the_heap );

  the_heap->start          = the_block;
  the_heap->first          = the_block;
  the_heap->permanent_null = NULL;
  the_heap->last           = the_block;

  the_block             = _Heap_Next_block( the_block );
  the_block->back_flag  = the_size;
  the_block->front_flag = HEAP_DUMMY_FLAG;
  the_heap->final       = the_block;

  return ( the_size - HEAP_BLOCK_USED_OVERHEAD );
}

/*PAGE
 *
 *  _Heap_Extend
 *
 *  This routine grows the_heap memory area using the size bytes which
 *  begin at starting_address.
 *
 *  Input parameters:
 *    the_heap          - pointer to heap header.
 *    starting_address  - pointer to the memory area.
 *    size              - size in bytes of the memory block to allocate.
 *
 *  Output parameters:
 *    *amount_extended  - amount of memory added to the_heap
 */

Heap_Extend_status _Heap_Extend(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32           size,
  unsigned32          *amount_extended
)
{
  Heap_Block        *the_block;
  unsigned32        *p;
  
  /*
   *  The overhead was taken from the original heap memory.
   */

  Heap_Block  *old_final;
  Heap_Block  *new_final;

  /*
   *  There are five possibilities for the location of starting
   *  address:
   *
   *    1. non-contiguous lower address     (NOT SUPPORTED)
   *    2. contiguous lower address         (NOT SUPPORTED)
   *    3. in the heap                      (ERROR)
   *    4. contiguous higher address        (SUPPORTED)
   *    5. non-contiguous higher address    (NOT SUPPORTED)
   *
   *  As noted, this code only supports (4).
   */

  if ( starting_address >= (void *) the_heap->start &&        /* case 3 */
       starting_address <= (void *) the_heap->final
     )
    return HEAP_EXTEND_ERROR;

  if ( starting_address < (void *) the_heap->start ) {  /* cases 1 and 2 */

      return HEAP_EXTEND_NOT_IMPLEMENTED;               /* cases 1 and 2 */

  } else {                                              /* cases 4 and 5 */

    the_block  = (Heap_Block *) (starting_address - HEAP_OVERHEAD);
    if ( the_block != the_heap->final )
      return HEAP_EXTEND_NOT_IMPLEMENTED;                   /* case 5 */
  }

  /*
   *  Currently only case 4 should make it to this point.
   *  The basic trick is to make the extend area look like a used
   *  block and free it.
   */

  *amount_extended = size;

  old_final = the_heap->final;
  new_final = _Addresses_Add_offset( old_final, size );
  /* SAME AS: _Addresses_Add_offset( starting_address, size-HEAP_OVERHEAD ); */

  the_heap->final = new_final;

  old_final->front_flag = 
  new_final->back_flag  = _Heap_Build_flag( size, HEAP_BLOCK_USED );
  new_final->front_flag = HEAP_DUMMY_FLAG;

  /*
   *  Must pass in address of "user" area
   *  So add in the offset field.
   */

  p = (unsigned32 *) &old_final->next;
  *p = sizeof(unsigned32);
  p++;
  _Heap_Free( the_heap, p );
  
  return HEAP_EXTEND_SUCCESSFUL;
}

/*PAGE
 *
 *  _Heap_Allocate
 *
 *  This kernel routine allocates the requested size of memory
 *  from the specified heap.
 *
 *  Input parameters:
 *    the_heap  - pointer to heap header.
 *    size      - size in bytes of the memory block to allocate.
 *
 *  Output parameters:
 *    returns - starting address of memory block allocated
 */

void *_Heap_Allocate(
  Heap_Control        *the_heap,
  unsigned32           size
)
{
  unsigned32  excess;
  unsigned32  the_size;
  Heap_Block *the_block;
  Heap_Block *next_block;
  Heap_Block *temporary_block;
  void       *ptr;
  unsigned32  offset;
  
  excess   = size % the_heap->page_size;
  the_size = size + the_heap->page_size + HEAP_BLOCK_USED_OVERHEAD;
  
  if ( excess )
    the_size += the_heap->page_size - excess;

  if ( the_size < sizeof( Heap_Block ) )
    the_size = sizeof( Heap_Block );

  for ( the_block = the_heap->first;
        ;
        the_block = the_block->next ) {
    if ( the_block == _Heap_Tail( the_heap ) )
      return( NULL );
    if ( the_block->front_flag >= the_size )
      break;
  }

  if ( (the_block->front_flag - the_size) >
       (the_heap->page_size + HEAP_BLOCK_USED_OVERHEAD) ) {
    the_block->front_flag -= the_size;
    next_block             = _Heap_Next_block( the_block );
    next_block->back_flag  = the_block->front_flag;

    temporary_block            = _Heap_Block_at( next_block, the_size );
    temporary_block->back_flag =
    next_block->front_flag     = _Heap_Build_flag( the_size,
                                    HEAP_BLOCK_USED );
    ptr = _Heap_Start_of_user_area( next_block );
  } else {
    next_block                = _Heap_Next_block( the_block );
    next_block->back_flag     = _Heap_Build_flag( the_block->front_flag,
                                   HEAP_BLOCK_USED );
    the_block->front_flag     = next_block->back_flag;
    the_block->next->previous = the_block->previous;
    the_block->previous->next = the_block->next;
    ptr = _Heap_Start_of_user_area( the_block );
  }
  
  /*
   * round ptr up to a multiple of page size
   * Have to save the bump amount in the buffer so that free can figure it out
   */
  
  offset = the_heap->page_size - (((unsigned32) ptr) & (the_heap->page_size - 1));
  ptr += offset;
  *(((unsigned32 *) ptr) - 1) = offset;

#ifdef RTEMS_DEBUG
  {
      unsigned32 ptr_u32;
      ptr_u32 = (unsigned32) ptr;
      if (ptr_u32 & (the_heap->page_size - 1))
          abort();
  }
#endif

  return ptr;
}

/*PAGE
 *
 *  _Heap_Size_of_user_area
 *
 *  This kernel routine returns the size of the memory area
 *  given heap block.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of the memory block to free.
 *    size             - pointer to size of area
 *
 *  Output parameters:
 *    size  - size of area filled in
 *    TRUE  - if starting_address is valid heap address
 *    FALSE - if starting_address is invalid heap address
 */

boolean _Heap_Size_of_user_area(
  Heap_Control        *the_heap,
  void                *starting_address,
  unsigned32          *size
)
{
  Heap_Block        *the_block;
  Heap_Block        *next_block;
  unsigned32         the_size;

  the_block = _Heap_User_Block_at( starting_address );
  
  if ( !_Heap_Is_block_in( the_heap, the_block ) ||
        _Heap_Is_block_free( the_block ) )
    return( FALSE );

  the_size   = _Heap_Block_size( the_block );
  next_block = _Heap_Block_at( the_block, the_size );

  if ( !_Heap_Is_block_in( the_heap, next_block ) ||
       (the_block->front_flag != next_block->back_flag) )
    return( FALSE );

  *size = the_size;
  return( TRUE );
}

/*PAGE
 *
 *  _Heap_Free
 *
 *  This kernel routine returns the memory designated by the
 *  given heap and given starting address to the memory pool.
 *
 *  Input parameters:
 *    the_heap         - pointer to heap header
 *    starting_address - starting address of the memory block to free.
 *
 *  Output parameters:
 *    TRUE  - if starting_address is valid heap address
 *    FALSE - if starting_address is invalid heap address
 */

boolean _Heap_Free(
  Heap_Control        *the_heap,
  void                *starting_address
)
{
  Heap_Block        *the_block;
  Heap_Block        *next_block;
  Heap_Block        *new_next_block;
  Heap_Block        *previous_block;
  Heap_Block        *temporary_block;
  unsigned32         the_size;

  the_block = _Heap_User_Block_at( starting_address );

  if ( !_Heap_Is_block_in( the_heap, the_block ) ||
        _Heap_Is_block_free( the_block ) ) {
      return( FALSE );
  }

  the_size   = _Heap_Block_size( the_block );
  next_block = _Heap_Block_at( the_block, the_size );

  if ( !_Heap_Is_block_in( the_heap, next_block ) ||
       (the_block->front_flag != next_block->back_flag) ) {
      return( FALSE );
  }

  if ( _Heap_Is_previous_block_free( the_block ) ) {
    previous_block = _Heap_Previous_block( the_block );

    if ( !_Heap_Is_block_in( the_heap, previous_block ) ) {
        return( FALSE );
    }

    if ( _Heap_Is_block_free( next_block ) ) {      /* coalesce both */
      previous_block->front_flag += next_block->front_flag + the_size;
      temporary_block             = _Heap_Next_block( previous_block );
      temporary_block->back_flag  = previous_block->front_flag;
      next_block->next->previous  = next_block->previous;
      next_block->previous->next  = next_block->next;
    }
    else {                     /* coalesce prev */
      previous_block->front_flag =
      next_block->back_flag      = previous_block->front_flag + the_size;
    }
  }
  else if ( _Heap_Is_block_free( next_block ) ) { /* coalesce next */
    the_block->front_flag     = the_size + next_block->front_flag;
    new_next_block            = _Heap_Next_block( the_block );
    new_next_block->back_flag = the_block->front_flag;
    the_block->next           = next_block->next;
    the_block->previous       = next_block->previous;
    next_block->previous->next = the_block;
    next_block->next->previous = the_block;

    if (the_heap->first == next_block)
        the_heap->first = the_block;
  }
  else {                          /* no coalesce */
    next_block->back_flag     =
    the_block->front_flag     = the_size;
    the_block->previous       = _Heap_Head( the_heap );
    the_block->next           = the_heap->first;
    the_heap->first           = the_block;
    the_block->next->previous = the_block;
  }

  return( TRUE );
}

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
  }

  while (notdone) {
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
        printf("PASS: %d  Front and back flags don't match\n", source);
        printf("         Current Block:  Back - %d,  Front - %d",
               the_block->back_flag, the_block->front_flag);
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
        printf("         Next Block:     Back - %d,  Front - %d",
               next_block->back_flag, next_block->front_flag);
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
}
