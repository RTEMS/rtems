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

#include <rtems/rtems/region.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/debug.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicRegionImpl Classic Region Manager Implementation
 *
 * @ingroup ClassicRegion
 *
 * @{
 */

/**
 *  @brief Instantiate RTEMS Region Data
 *
 *  Region Manager -- Instantiate Data
 *
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_REGION_EXTERN
#define RTEMS_REGION_EXTERN extern
#endif

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
RTEMS_REGION_EXTERN Objects_Information _Region_Information;

/**
 *  @brief _Region_Manager_initialization
 *
 *  Region Manager
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Region_Manager_initialization(void);

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

/**
 *  @brief Region_Get
 *
 *  This function maps region IDs to region control blocks.
 *  If ID corresponds to a local region, then it returns
 *  the_region control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_region is undefined.
 */
RTEMS_INLINE_ROUTINE Region_Control *_Region_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Region_Control *)
    _Objects_Get_no_protection( &_Region_Information, id, location );
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

/**
 *  @brief _Region_Debug_Walk
 *
 *  This routine is invoked to verify the integrity of a heap associated
 *  with the_region.
 */
#ifdef RTEMS_DEBUG

#define _Region_Debug_Walk( _the_region, _source ) \
  do { \
    if ( rtems_debug_is_enabled( RTEMS_DEBUG_REGION ) ) \
      _Heap_Walk( &(_the_region)->Memory, _source, false ); \
  } while ( 0 )

#else

#define _Region_Debug_Walk( _the_region, _source )

#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/regionmp.h>
#endif

#endif
/* end of include file */
