/**
 * @file
 *
 * @brief Check if the specified API mutex is locked.
 *
 * @ingroup ScoreAPIMutex
 */

/*
 *  COPYRIGHT (c) 1989-2015.
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

bool _API_Mutex_Is_Locked( API_Mutex_Control *the_mutex )
{
  bool      is_locked;
  ISR_Level level;

  _ISR_Disable( level );
    is_locked = _CORE_mutex_Is_locked( &the_mutex->Mutex );
  _ISR_Enable( level );

  return is_locked;
}
