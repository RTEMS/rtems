/*  wkspace.inl
 *
 *  This include file contains the bodies of the routines which contains
 *  information related to the RAM Workspace.
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
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

STATIC INLINE void _Workspace_Handler_initialization(
  void       *starting_address,
  unsigned32  size
)
{
  unsigned32 *zero_out_array;
  unsigned32  index;
  unsigned32  memory_available;

  if ( !starting_address || !_Addresses_Is_aligned( starting_address ) )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS
    );

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
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
    );
}

/*PAGE
 *
 *  _Workspace_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then NULL is returned.
 */

STATIC INLINE void *_Workspace_Allocate(
  unsigned32 size
)
{
   return _Heap_Allocate( &_Workspace_Area, size );
}

/*PAGE
 *
 *  _Workspace_Free
 *
 *  DESCRIPTION:
 *
 *  This function frees the specified block of memory.  If the block
 *  belongs to the Workspace and can be successfully freed, then
 *  TRUE is returned.  Otherwise FALSE is returned.
 */

STATIC INLINE boolean _Workspace_Free(
  void *block
)
{
   return _Heap_Free( &_Workspace_Area, block );
}

#endif
/* end of include file */
