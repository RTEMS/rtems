/*  macros/coresem.h
 *
 *  This include file contains all of the inlined routines associated
 *  with core semaphores.
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

#ifndef __MACROS_CORE_SEMAPHORE_h
#define __MACROS_CORE_SEMAPHORE_h

/*PAGE
 *
 *  _CORE_semaphore_Is_priority
 *
 */

#define _CORE_semaphore_Is_priority( _the_attribute ) \
  ( (_the_attribute)->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY )

/*PAGE
 *
 *  _Core_semaphore_Get_count
 *
 */
 
#define _Core_semaphore_Get_count( _the_semaphore ) \
  ( (_the_semaphore)->count )

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

#define _CORE_semaphore_Seize_isr_disable( \
  _the_semaphore, _id, _wait, _timeout, _level_p) \
{ \
  Thread_Control *executing; \
  ISR_Level       level = *(_level_p); \
 \
  /* disabled when you get here */ \
 \
  executing = _Thread_Executing; \
  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL; \
  if ( (_the_semaphore)->count != 0 ) { \
    (_the_semaphore)->count -= 1; \
    _ISR_Enable( level ); \
  } else if ( !(_wait) ) { \
    _ISR_Enable( level ); \
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT; \
  } else { \
    _Thread_Disable_dispatch(); \
    _ISR_Enable( level ); \
    _Thread_queue_Enter_critical_section( &(_the_semaphore)->Wait_queue ); \
    executing->Wait.queue          = &(_the_semaphore)->Wait_queue; \
    executing->Wait.id             = (_id); \
    _ISR_Enable( level ); \
 \
    _Thread_queue_Enqueue( &(_the_semaphore)->Wait_queue, (_timeout) ); \
    _Thread_Enable_dispatch(); \
  } \
}


#endif
/* end of include file */
