/*  inline/coremutex.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the CORE mutexes.
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

#ifndef __INLINE_CORE_MUTEX_inl
#define __INLINE_CORE_MUTEX_inl

/*PAGE
 *
 *  _CORE_mutex_Is_locked
 *
 */
 
STATIC INLINE boolean _CORE_mutex_Is_locked(
  CORE_mutex_Control  *the_mutex
)
{
  return the_mutex->lock == CORE_MUTEX_LOCKED;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_fifo
 *
 */
 
STATIC INLINE boolean _CORE_mutex_Is_fifo(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_FIFO;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_priority
 *
 */
 
STATIC INLINE boolean _CORE_mutex_Is_priority(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_inherit_priority
 *
 */
 
STATIC INLINE boolean _CORE_mutex_Is_inherit_priority(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_priority_ceiling
 *
 */
 
STATIC INLINE boolean _CORE_mutex_Is_priority_ceiling(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_nesting_allowed
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex allows a task to obtain a
 *  semaphore more than once and nest.
 */
 
STATIC INLINE boolean _CORE_mutex_Is_nesting_allowed(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->allow_nesting == TRUE;
 
}
 
#endif
/* end of include file */
