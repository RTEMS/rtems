/*  inline/coremutex.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the CORE mutexes.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __INLINE_CORE_MUTEX_inl
#define __INLINE_CORE_MUTEX_inl

/*PAGE
 *
 *  _CORE_mutex_Is_locked
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex specified is locked and FALSE
 *  otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_locked(
  CORE_mutex_Control  *the_mutex
)
{
  return the_mutex->lock == CORE_MUTEX_LOCKED;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_fifo
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is FIFO and FALSE
 *  otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_fifo(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_FIFO;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is PRIORITY and
 *  FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_priority(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_inherit_priority
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is
 *  INHERIT_PRIORITY and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_inherit_priority(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
}
 
/*PAGE
 *
 *  _CORE_mutex_Is_priority_ceiling
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is
 *  PRIORITY_CEILING and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_priority_ceiling(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
}
 
#endif
/* end of include file */
