/*  wkspace.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the RTEMS RAM Workspace Handler.
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

#include <rtems/heap.h>
#include <rtems/fatal.h>
#include <rtems/status.h>

/*PAGE
 *
 *  _Workspace_Handler_initialization
 *
 */

#define _Workspace_Handler_initialization( _starting_address, _size ) \
{ \
  unsigned32 *zero_out_array; \
  unsigned32  index; \
  unsigned32  memory_available; \
  \
  if ( ((_starting_address) == NULL) ||  \
       !_Addresses_Is_aligned( (_starting_address) ) )  \
    rtems_fatal_error_occurred( RTEMS_INVALID_ADDRESS ); \
  \
  if ( _CPU_Table.do_zero_of_workspace ) { \
    for( zero_out_array  = (unsigned32 *) (_starting_address), index = 0 ; \
         index < (_size) / 4 ; \
         index++ ) \
      zero_out_array[ index ] = 0; \
  } \
  \
  memory_available =  _Heap_Initialize( \
    &_Workspace_Area, \
    (_starting_address), \
    (_size), \
    CPU_ALIGNMENT \
  ); \
  \
  if ( memory_available == 0 ) \
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED ); \
}

/*PAGE
 *
 *  _Workspace_Allocate
 *
 */

#define _Workspace_Allocate( _size ) \
   _Heap_Allocate( &_Workspace_Area, (_size) )

/*PAGE
 *
 *  _Workspace_Free
 *
 */

#define _Workspace_Free( _block ) \
   _Heap_Free( &_Workspace_Area, (_block) )

#endif
/* end of include file */
