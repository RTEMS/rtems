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
  bool previous_thread_life_protection;
  ISR_Level level;

  previous_thread_life_protection = _Thread_Set_life_protection( true );

  #if defined(RTEMS_SMP)
    _Thread_Disable_dispatch();
  #endif

  _ISR_Disable( level );

  _CORE_mutex_Seize(
    &the_mutex->Mutex,
    _Thread_Executing,
    the_mutex->Object.id,
    true,
    0,
    level
  );

  if ( the_mutex->Mutex.nest_count == 1 ) {
    the_mutex->previous_thread_life_protection =
      previous_thread_life_protection;
  }

  #if defined(RTEMS_SMP)
    _Thread_Enable_dispatch();
  #endif
}
