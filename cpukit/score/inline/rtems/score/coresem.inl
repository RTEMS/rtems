/*  inline/coresem.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the CORE semaphore.
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

STATIC INLINE boolean _CORE_semaphore_Is_priority(
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
 
STATIC INLINE unsigned32 _CORE_semaphore_Get_count(
  CORE_semaphore_Control  *the_semaphore
)
{
  return the_semaphore->count;
}

#endif
/* end of include file */
