/*  inline/objectmp.inl
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

#ifndef __INLINE_MP_OBJECTS_inl
#define __INLINE_MP_OBJECTS_inl

/*PAGE
 *
 *  _Objects_MP_Allocate_global_object
 *
 *  DESCRIPTION:
 *
 *  This function allocates a Global Object control block.
 */

RTEMS_INLINE_ROUTINE Objects_MP_Control *_Objects_MP_Allocate_global_object (
  void
)
{
  return (Objects_MP_Control *)
           _Chain_Get( &_Objects_MP_Inactive_global_objects );
}

/*PAGE
 *
 *  _Objects_MP_Free_global_object
 *
 *  DESCRIPTION:
 *
 *  This routine deallocates a Global Object control block.
 */

RTEMS_INLINE_ROUTINE void _Objects_MP_Free_global_object (
  Objects_MP_Control *the_object
)
{
  _Chain_Append(
    &_Objects_MP_Inactive_global_objects,
    &the_object->Object.Node
  );
}

/*PAGE
 *
 *  _Objects_MP_Is_null_global_object
 *
 *  DESCRIPTION:
 *
 *  This function returns whether the global object is NULL or not.
 */

RTEMS_INLINE_ROUTINE boolean _Objects_MP_Is_null_global_object (
  Objects_MP_Control *the_object
)
{
  return( the_object == NULL );
}

#endif
/* end of include file */
