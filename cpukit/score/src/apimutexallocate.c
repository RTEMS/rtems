/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/apimutex.h>

void _API_Mutex_Allocate(
  API_Mutex_Control **the_mutex
)
{
  API_Mutex_Control *mutex;

  CORE_mutex_Attributes attr =  {
    CORE_MUTEX_NESTING_ACQUIRES,
    false,
    CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT,
    0
  };

  mutex = (API_Mutex_Control *) _Objects_Allocate( &_API_Mutex_Information );

  _CORE_mutex_Initialize( &mutex->Mutex, &attr, CORE_MUTEX_UNLOCKED );

  _Objects_Open_u32( &_API_Mutex_Information, &mutex->Object, 1 );

  *the_mutex = mutex;
}
