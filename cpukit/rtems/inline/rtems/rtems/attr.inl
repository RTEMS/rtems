/**
 * @file rtems/rtems/attr.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with attributes.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_ATTR_INL
#define _RTEMS_RTEMS_ATTR_INL

/**
 *  @addtogroup ClassicAttributes
 *  @{
 */

/**
 *  @brief Attributes_Set
 *
 *  This function sets the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */
RTEMS_INLINE_ROUTINE rtems_attribute _Attributes_Set (
   rtems_attribute new_attributes,
   rtems_attribute attribute_set
)
{
  return attribute_set | new_attributes;
}

/**
 *  @brief Attributes_Clear
 *
 *  This function clears the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */
RTEMS_INLINE_ROUTINE rtems_attribute _Attributes_Clear (
   rtems_attribute attribute_set,
   rtems_attribute mask
)
{
  return attribute_set & ~mask;
}

/**
 *  @brief Attributes_Is_floating_point
 *
 *  This function returns TRUE if the floating point attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_floating_point(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_FLOATING_POINT );
}

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief Attributes_Is_global
 *
 *  This function returns TRUE if the global object attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_global(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_GLOBAL );
}
#endif

/**
 *  @brief Attributes_Is_priority
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY );
}

/**
 *  @brief Attributes_Is_binary_semaphore
 *
 *  This function returns TRUE if the binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_binary_semaphore(
  rtems_attribute attribute_set
)
{
  return ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_BINARY_SEMAPHORE);
}

/**
 *  @brief Attributes_Is_simple_binary_semaphore
 *
 *  This function returns TRUE if the simple binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_simple_binary_semaphore(
  rtems_attribute attribute_set
) 
{
  return
    ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_SIMPLE_BINARY_SEMAPHORE);
}  

/**
 *  @brief Attributes_Is_counting_semaphore
 *
 *  This function returns TRUE if the counting semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_counting_semaphore(
  rtems_attribute attribute_set
)
{
  return ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_COUNTING_SEMAPHORE);
}

/**
 *  @brief Attributes_Is_inherit_priority
 *
 *  This function returns TRUE if the priority inheritance attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_inherit_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_INHERIT_PRIORITY );
}

/**
 *  @brief Attributes_Is_priority_ceiling
 *
 *  This function returns TRUE if the priority ceiling attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_priority_ceiling(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY_CEILING );
}

/**
 *  @brief Attributes_Is_barrier_automatic
 *
 *  This function returns TRUE if the barrier automatic release
 *  attribute is enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_barrier_automatic(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_BARRIER_AUTOMATIC_RELEASE );
}

/**
 *  @brief Attributes_Is_system_task
 *
 *  This function returns TRUE if the system task attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Attributes_Is_system_task(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_SYSTEM_TASK );
}

/**@}*/

#endif
/* end of include file */
