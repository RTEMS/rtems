/*  inline/attr.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with attributes.
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

#ifndef __INLINE_ATTRIBUTES_inl
#define __INLINE_ATTRIBUTES_inl

/*PAGE
 *
 *  _Attributes_Set
 *
 *  DESCRIPTION:
 *
 *  This function sets the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */

STATIC INLINE rtems_attribute _Attributes_Set (
   rtems_attribute new_attributes,
   rtems_attribute attribute_set
)
{
  return attribute_set | new_attributes;
}

/*PAGE
 *
 *  _Attributes_Clear
 *
 *  DESCRIPTION:
 *
 *  This function clears the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */

STATIC INLINE rtems_attribute _Attributes_Clear (
   rtems_attribute attribute_set,
   rtems_attribute mask
)
{
  return attribute_set & ~mask;
}

/*PAGE
 *
 *  _Attributes_Is_floating_point
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the floating point attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_floating_point(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_FLOATING_POINT );
}

/*PAGE
 *
 *  _Attributes_Is_global
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the global object attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_global(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_GLOBAL );
}

/*PAGE
 *
 *  _Attributes_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY );
}

/*PAGE
 *
 *  _Attributes_Is_binary_semaphore
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_binary_semaphore(
  rtems_attribute attribute_set
)
{
  return ( attribute_set & RTEMS_BINARY_SEMAPHORE );
}

/*PAGE
 *
 *  _Attributes_Is_inherit_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority inheritance attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_inherit_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_INHERIT_PRIORITY );
}

/*PAGE
 *
 *  _Attributes_Is_priority_ceiling
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority ceiling attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
 
STATIC INLINE boolean _Attributes_Is_priority_ceiling(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY_CEILING );
}

#endif
/* end of include file */
