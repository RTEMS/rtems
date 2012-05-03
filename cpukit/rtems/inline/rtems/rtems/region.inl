/**
 * @file rtems/rtems/region.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Region Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_REGION_H
# error "Never use <rtems/rtems/region.inl> directly; include <rtems/rtems/region.h> instead."
#endif

#ifndef _RTEMS_RTEMS_REGION_INL
#define _RTEMS_RTEMS_REGION_INL

/**
 *  @addtogroup ClassicRegion
 *  @{
 */

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
 *  @brief Region_Is_null
 *
 *  This function returns TRUE if the_region is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Region_Is_null (
  Region_Control *the_region
)
{
  return ( the_region == NULL  );
}

/**@}*/

#endif
/* end of include file */
