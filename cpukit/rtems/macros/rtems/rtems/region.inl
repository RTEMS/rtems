/*  region.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Region Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#define _Region_Allocate() \
  (Region_Control *) _Objects_Allocate( &_Region_Information )

/*PAGE
 *
 *  _Region_Free
 *
 */

#define _Region_Free( _the_region ) \
  _Objects_Free( &_Region_Information, &(_the_region)->Object )

/*PAGE
 *
 *  _Region_Get
 *
 */

#define _Region_Get( _id, _location ) \
  (Region_Control *) \
   _Objects_Get_no_protection( &_Region_Information, (_id), (_location) )

/*PAGE
 *
 *  _Region_Allocate_segment
 *
 */

#define _Region_Allocate_segment( _the_region, _size ) \
   _Heap_Allocate( &(_the_region)->Memory, (_size) )

/*PAGE
 *
 *  _Region_Free_segment
 *
 */

#define _Region_Free_segment( _the_region, _the_segment ) \
   _Heap_Free( &(_the_region)->Memory, (_the_segment) )

/*PAGE
 *
 *  _Region_Is_null
 *
 */

#define _Region_Is_null( _the_region )   ( (_the_region) == NULL  )

#endif
/* end of include file */
