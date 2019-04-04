/**
 * @file
 *
 * @brief Acquires the specified API mutex.
 *
 * @ingroup RTEMSScoreAPIMutex
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
#include <rtems/score/threadimpl.h>

void _API_Mutex_Lock( API_Mutex_Control *the_mutex )
{
  Thread_Life_state previous_thread_life_state;

  previous_thread_life_state =
    _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );

  _Mutex_recursive_Acquire( &the_mutex->Mutex );

  if ( the_mutex->Mutex._nest_level == 0 ) {
    the_mutex->previous_thread_life_state = previous_thread_life_state;
  }
}
