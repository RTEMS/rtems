/*  inline/coresem.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the CORE semaphore.
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

#ifndef __INLINE_CORE_SEMAPHORE_inl
#define __INLINE_CORE_SEMAPHORE_inl

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

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

/*PAGE
 *
 *  _CORE_semaphore_Seize_isr_disable
 *
 *  DESCRIPTION:
 *
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  NOTE: There is currently no MACRO version of this routine.
 */
 
RTEMS_INLINE_ROUTINE void _CORE_semaphore_Seize_isr_disable(
  CORE_semaphore_Control  *the_semaphore,
  Objects_Id               id,
  boolean                  wait,
  Watchdog_Interval        timeout,
  ISR_Level               *level_p
) 
{ 
  Thread_Control *executing;
  ISR_Level       level = *level_p;

  /* disabled when you get here */
  
  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _ISR_Enable( level );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_Disable_dispatch();
  _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
  executing->Wait.queue          = &the_semaphore->Wait_queue;
  executing->Wait.id             = id;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
  _Thread_Enable_dispatch();
}

#endif
/* end of include file */
