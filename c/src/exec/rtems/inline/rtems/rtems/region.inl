/*  region.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Region Manager.
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

#ifndef __REGION_inl
#define __REGION_inl

/*PAGE
 *
 *  _Region_Allocate
 *
 */

STATIC INLINE Region_Control *_Region_Allocate( void )
{
  return (Region_Control *) _Objects_Allocate( &_Region_Information );
}

/*PAGE
 *
 *  _Region_Free
 *
 */

STATIC INLINE void _Region_Free (
  Region_Control *the_region
)
{
  _Objects_Free( &_Region_Information, &the_region->Object );
}

/*PAGE
 *
 *  _Region_Get
 *
 */

STATIC INLINE Region_Control *_Region_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Region_Control *)
    _Objects_Get( &_Region_Information, id, location );
}

/*PAGE
 *
 *  _Region_Allocate_segment
 *
 */

STATIC INLINE void *_Region_Allocate_segment (
  Region_Control *the_region,
  unsigned32      size
)
{
   return _Heap_Allocate( &the_region->Memory, size );
}

/*PAGE
 *
 *  _Region_Free_segment
 *
 */

STATIC INLINE boolean _Region_Free_segment (
  Region_Control *the_region,
  void           *the_segment
)
{
  return _Heap_Free( &the_region->Memory, the_segment );
}

/*PAGE
 *
 *  _Region_Is_null
 *
 */

STATIC INLINE boolean _Region_Is_null (
  Region_Control *the_region
)
{
  return ( the_region == NULL  );
}

#endif
/* end of include file */
