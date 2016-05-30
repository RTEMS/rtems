/**
 * @file
 *
 * @brief Acquires the specified API mutex.
 *
 * @ingroup ScoreAPIMutex
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/apimutex.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/threadimpl.h>

void _API_Mutex_Lock( API_Mutex_Control *the_mutex )
{
  Thread_Life_state    previous_thread_life_state;
  Thread_queue_Context queue_context;

  previous_thread_life_state =
    _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );

  _Thread_queue_Context_initialize( &queue_context );
  _ISR_lock_ISR_disable( &queue_context.Lock_context );

  _CORE_recursive_mutex_Seize(
    &the_mutex->Mutex,
    _Thread_Executing,
    true,
    WATCHDOG_NO_TIMEOUT,
    _CORE_recursive_mutex_Seize_nested,
    &queue_context
  );

  if ( the_mutex->Mutex.nest_level == 0 ) {
    the_mutex->previous_thread_life_state = previous_thread_life_state;
  }
}
