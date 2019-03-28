/**
 * @file
 *
 * @ingroup ClassicRegionImpl
 *
 * @brief Classic Region Manager Implementation
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_REGIONIMPL_H
#define _RTEMS_RTEMS_REGIONIMPL_H

#include <rtems/rtems/regiondata.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicRegionImpl Classic Region Manager Implementation
 *
 * @ingroup RTEMSInternalClassic
 *
 * @{
 */

#define REGION_OF_THREAD_QUEUE_QUEUE( queue ) \
  RTEMS_CONTAINER_OF( queue, Region_Control, Wait_queue.Queue )

/**
 *  @brief Region_Allocate
 *
 *  This function allocates a region control block from
 *  the inactive chain of free region control blocks.
 */
RTEMS_INLINE_ROUTINE Region_Control *_Region_Allocate( void )
{
  return (Region_Control *) _Objects_Allocate( &_Region_Information );
}

/**
 *  @brief Region_Free
 *
 *  This routine frees a region control block to the
 *  inactive chain of free region control blocks.
 */
RTEMS_INLINE_ROUTINE void _Region_Free (
  Region_Control *the_region
)
{
  _Thread_queue_Destroy( &the_region->Wait_queue );
  _Objects_Free( &_Region_Information, &the_region->Object );
}

RTEMS_INLINE_ROUTINE Region_Control *_Region_Get_and_lock( Objects_Id id )
{
  Region_Control *the_region;

  _RTEMS_Lock_allocator();

  the_region = (Region_Control *)
    _Objects_Get_no_protection( id, &_Region_Information );

  if ( the_region != NULL ) {
    /* Keep allocator lock */
    return the_region;
  }

  _RTEMS_Unlock_allocator();
  return NULL;
}

RTEMS_INLINE_ROUTINE void _Region_Unlock( Region_Control *the_region )
{
  (void) the_region;
  _RTEMS_Unlock_allocator();
}

/**
 *  @brief Region_Allocate_segment
 *
 *  This function attempts to allocate a segment from the_region.
 *  If successful, it returns the address of the allocated segment.
 *  Otherwise, it returns NULL.
 */
RTEMS_INLINE_ROUTINE void *_Region_Allocate_segment (
  Region_Control *the_region,
  uintptr_t       size
)
{
  return _Heap_Allocate( &the_region->Memory, size );
}

/**
 *  @brief Region_Free_segment
 *
 *  This function frees the_segment to the_region.
 */
RTEMS_INLINE_ROUTINE bool _Region_Free_segment (
  Region_Control *the_region,
  void           *the_segment
)
{
  return _Heap_Free( &the_region->Memory, the_segment );
}

/**
 *  @brief Process Region Queue
 *
 *  This is a helper routine which is invoked any time memory is
 *  freed.  It looks at the set of waiting tasks and attempts to
 *  satisfy all outstanding requests.
 *
 *  @param[in] the_region is the the region
 */
extern void _Region_Process_queue(Region_Control *the_region);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
