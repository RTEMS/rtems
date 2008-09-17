/*
 *  RTEMS Malloc Family Implementation --Initialization
 *
 *
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/wkspace.h>
#include "malloc_p.h"

rtems_malloc_statistics_t rtems_malloc_statistics;
extern bool rtems_unified_work_area;

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
)
{
  uint32_t      status;
  void         *starting_address;

  #if defined(RTEMS_MALLOC_BOUNDARY_HELPERS)
    /*
     *  If configured, initialize the boundary support
     */
    if (rtems_malloc_boundary_helpers)
      (*rtems_malloc_boundary_helpers->initialize)();
  #endif

  /*
   *  If configured, initialize the statistics support
   */
  if ( rtems_malloc_statistics_helpers )
    (*rtems_malloc_statistics_helpers->initialize)();

  /*
   *  Initialize the garbage collection list to start with nothing on it.
   */
  malloc_deferred_frees_initialize();

  starting_address = start;

  /*
   *  Initialize the optional sbrk support for extending the heap
   */
  if (rtems_malloc_sbrk_helpers) {
    starting_address = (*rtems_malloc_sbrk_helpers->initialize)(
      start,
      sbrk_amount
    );
  }

  /*
   *  If this system is configured to use the same heap for
   *  the RTEMS Workspace and C Program Heap, then we need to
   *  be very very careful about destroying the initialization
   *  that has already been done.
   */
    
  /*
   *  If the BSP is not clearing out the workspace, then it is most likely
   *  not clearing out the initial memory for the heap.  There is no
   *  standard supporting zeroing out the heap memory.  But much code
   *  with UNIX history seems to assume that memory malloc'ed during
   *  initialization (before any free's) is zero'ed.  This is true most
   *  of the time under UNIX because zero'ing memory when it is first
   *  given to a process eliminates the chance of a process seeing data
   *  left over from another process.  This would be a security violation.
   */

  if ( !rtems_unified_work_area &&
       rtems_configuration_get_do_zero_of_workspace() )
     memset( starting_address, 0, length );

  /*
   *  Unfortunately we cannot use assert if this fails because if this
   *  has failed we do not have a heap and if we do not have a heap
   *  STDIO cannot work because there will be no buffers.
   */

  if ( !rtems_unified_work_area ) {
    status = _Protected_heap_Initialize( 
      RTEMS_Malloc_Heap,
      starting_address,
      length,
      CPU_HEAP_ALIGNMENT
    );
    if ( !status )
      rtems_fatal_error_occurred( status );
  }

  #if defined(RTEMS_HEAP_DEBUG)
    if ( _Protected_heap_Walk( RTEMS_Malloc_Heap, 0, false ) ) {
      printk( "Malloc heap not initialized correctly\n" );
      rtems_print_buffer( start, 32 );
      printk( "\n" );
      rtems_print_buffer( (start + length) - 48, 48 );
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
    }
  #endif

  MSBUMP(space_available, length);
}
