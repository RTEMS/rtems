/**
 * @file
 *
 * @brief Releases the Specified API Mutex
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

void _API_Mutex_Unlock( API_Mutex_Control *the_mutex )
{
  ISR_lock_Context lock_context;
  bool             previous_thread_life_protection;
  bool             restore_thread_life_protection;

  previous_thread_life_protection =
    the_mutex->previous_thread_life_protection;
  restore_thread_life_protection = the_mutex->Mutex.nest_count == 1;

  _ISR_lock_ISR_disable( &lock_context );
  _CORE_mutex_Surrender(
    &the_mutex->Mutex,
    the_mutex->Object.id,
    NULL,
    &lock_context
  );

  if ( restore_thread_life_protection ) {
    _Thread_Set_life_protection( previous_thread_life_protection );
  }
}
