/*  inline/coresem.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the CORE semaphore.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __INLINE_CORE_SEMAPHORE_inl
#define __INLINE_CORE_SEMAPHORE_inl

/*PAGE
 *
 *  _CORE_semaphore_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _CORE_semaphore_Is_priority(
  CORE_semaphore_Attributes *the_attribute
)
{
   return ( the_attribute->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY );
}

/*PAGE
 *
 *  _CORE_semaphore_Get_count
 *
 *  DESCRIPTION:
 *
 *  This routine returns the current count associated with the semaphore.
 */
 
RTEMS_INLINE_ROUTINE unsigned32 _CORE_semaphore_Get_count(
  CORE_semaphore_Control  *the_semaphore
)
{
  return the_semaphore->count;
}

#endif
/* end of include file */
