/** 
 *  @file  rtems/score/coresem.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the SuperCore semaphore.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESEM_H
# error "Never use <rtems/score/coresem.inl> directly; include <rtems/score/coresem.h> instead."
#endif

#ifndef _RTEMS_SCORE_CORESEM_INL
#define _RTEMS_SCORE_CORESEM_INL

/**
 *  @addtogroup ScoreSemaphore 
 *  @{
 */

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/**
 *  This function returns true if the priority attribute is
 *  enabled in the @a attribute_set and false otherwise.
 *
 *  @param[in] the_attribute is the attribute set to test
 *  @return true if the priority attribute is enabled
 */
RTEMS_INLINE_ROUTINE bool _CORE_semaphore_Is_priority(
  CORE_semaphore_Attributes *the_attribute
)
{
   return ( the_attribute->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY );
}

/**
 *  This routine returns the current count associated with the semaphore.
 *
 *  @param[in] the_semaphore is the semaphore to obtain the count of
 *  @return the current count of this semaphore
 */
RTEMS_INLINE_ROUTINE uint32_t  _CORE_semaphore_Get_count(
  CORE_semaphore_Control  *the_semaphore
)
{
  return the_semaphore->count;
}

/**
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the wait flag is false, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  @param[in] the_semaphore is the semaphore to obtain
 *  @param[in] id is the Id of the owning API level Semaphore object
 *  @param[in] wait is true if the thread is willing to wait
 *  @param[in] timeout is the maximum number of ticks to block
 *  @param[in] level_p is a temporary variable used to contain the ISR
 *         disable level cookie
 *
 *  @note There is currently no MACRO version of this routine.
 */
RTEMS_INLINE_ROUTINE void _CORE_semaphore_Seize_isr_disable(
  CORE_semaphore_Control  *the_semaphore,
  Objects_Id               id,
  bool                     wait,
  Watchdog_Interval        timeout,
  ISR_Level               *level_p
) 
{ 
  Thread_Control *executing;

  /* disabled when you get here */
  
  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _ISR_Enable( *level_p );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( *level_p );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_Disable_dispatch();
  _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
  executing->Wait.queue          = &the_semaphore->Wait_queue;
  executing->Wait.id             = id;
  _ISR_Enable( *level_p );

  _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
  _Thread_Enable_dispatch();
}

/**@}*/

#endif
/* end of include file */
