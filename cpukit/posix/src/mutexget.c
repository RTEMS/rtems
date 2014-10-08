/**
 * @file
 *
 * @brief Convert POSIX Mutex ID to local object pointer
 * @ingroup POSIXAPI
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

#include <rtems/posix/muteximpl.h>
#include <rtems/score/apimutex.h>

static bool _POSIX_Mutex_Check_id_and_auto_init(
  pthread_mutex_t   *mutex,
  Objects_Locations *location
)
{
  if ( mutex == NULL ) {
    *location = OBJECTS_ERROR;

    return false;
  }

  if ( *mutex == PTHREAD_MUTEX_INITIALIZER ) {
    int eno;

    _Once_Lock();

    if ( *mutex == PTHREAD_MUTEX_INITIALIZER ) {
      eno = pthread_mutex_init( mutex, NULL );
    } else {
      eno = 0;
    }

    _Once_Unlock();

    if ( eno != 0 ) {
      *location = OBJECTS_ERROR;

      return false;
    }
  }

  return true;
}

POSIX_Mutex_Control *_POSIX_Mutex_Get (
  pthread_mutex_t   *mutex,
  Objects_Locations *location
)
{
  if ( !_POSIX_Mutex_Check_id_and_auto_init( mutex, location ) ) {
    return NULL;
  }

  return (POSIX_Mutex_Control *)
    _Objects_Get( &_POSIX_Mutex_Information, (Objects_Id) *mutex, location );
}

POSIX_Mutex_Control *_POSIX_Mutex_Get_interrupt_disable (
  pthread_mutex_t   *mutex,
  Objects_Locations *location,
  ISR_Level         *level
)
{
  if ( !_POSIX_Mutex_Check_id_and_auto_init( mutex, location ) ) {
    return NULL;
  }

  return (POSIX_Mutex_Control *) _Objects_Get_isr_disable(
    &_POSIX_Mutex_Information,
    (Objects_Id) *mutex,
    location,
    level
  );
}
