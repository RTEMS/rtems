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
   _Objects_Get( &_Region_Information, (_id), (_location) )

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
