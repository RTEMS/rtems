/*  inline/objectmp.inl
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

STATIC INLINE Objects_MP_Control *_Objects_MP_Allocate_global_object (
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

STATIC INLINE void _Objects_MP_Free_global_object (
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

STATIC INLINE boolean _Objects_MP_Is_null_global_object (
  Objects_MP_Control *the_object
)
{
  return( the_object == NULL );
}

#endif
/* end of include file */
