/*  wkspace.inl
 *
 *  This include file contains the bodies of the routines which contains
 *  information related to the RTEMS RAM Workspace.
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

#ifndef __WORKSPACE_inl
#define __WORKSPACE_inl

/*PAGE
 *
 *  _Workspace_Handler_initialization
 *
 */

STATIC INLINE void _Workspace_Handler_initialization(
  void       *starting_address,
  unsigned32  size
)
{
  unsigned32 *zero_out_array;
  unsigned32  index;
  unsigned32  memory_available;

  if ( (starting_address == NULL) ||
       !_Addresses_Is_aligned( starting_address ) )
    rtems_fatal_error_occurred( RTEMS_INVALID_ADDRESS );

  if ( _CPU_Table.do_zero_of_workspace ) {
    for( zero_out_array  = (unsigned32 *) starting_address, index = 0 ;
         index < size / 4 ;
         index++ )
      zero_out_array[ index ] = 0;
  }

  memory_available = _Heap_Initialize(
    &_Workspace_Area,
    starting_address,
    size,
    CPU_HEAP_ALIGNMENT
  );

  if ( memory_available == 0 )
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED );
}

/*PAGE
 *
 *  _Workspace_Allocate
 *
 */

STATIC INLINE void *_Workspace_Allocate(
  unsigned32 size
)
{
   return _Heap_Allocate( &_Workspace_Area, size );
}

/*PAGE
 *
 *  _Workspace_Allocate_or_fatal_error
 *
 */

STATIC INLINE void *_Workspace_Allocate_or_fatal_error(
  unsigned32   size
)
{
  void        *memory;

  memory = _Workspace_Allocate( size );

  if ( memory == NULL )
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED );

  return memory;
}

/*PAGE
 *
 *  _Workspace_Free
 *
 */

STATIC INLINE boolean _Workspace_Free(
  void *block
)
{
   return _Heap_Free( &_Workspace_Area, block );
}

#endif
/* end of include file */
