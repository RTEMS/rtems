/*  macros/objectmp.inl
 *
 *  This include file contains the bodies of all inlined routines
 *  which deal with global objects.
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
