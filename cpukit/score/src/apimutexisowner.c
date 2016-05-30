/**
 * @file
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

bool _API_Mutex_Is_owner( const API_Mutex_Control *the_mutex )
{
  return _CORE_mutex_Is_owner(
    &the_mutex->Mutex.Mutex,
    _Thread_Get_executing()
  );
}
