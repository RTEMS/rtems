/*  macros/objectmp.inl
 *
 *  This include file contains the bodies of all inlined routines
 *  which deal with global objects.
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

#ifndef __MACROS_MP_OBJECTS_inl
#define __MACROS_MP_OBJECTS_inl

/*PAGE
 *
 *  _Objects_MP_Allocate_global_object
 *
 */

#define _Objects_MP_Allocate_global_object() \
  (Objects_MP_Control *) \
           _Chain_Get( &_Objects_MP_Inactive_global_objects )

/*PAGE
 *  _Objects_MP_Free_global_object
 *
 */

#define _Objects_MP_Free_global_object( _the_object ) \
  _Chain_Append( \
    &_Objects_MP_Inactive_global_objects, \
    &(_the_object)->Object.Node \
  )

/*PAGE
 *  _Objects_MP_Is_null_global_object
 *
 */

#define _Objects_MP_Is_null_global_object( _the_object ) \
  ( (_the_object) == NULL )

#endif
/* end of include file */
