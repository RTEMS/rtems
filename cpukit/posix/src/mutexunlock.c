/**
 * @file
 *
 * @brief Locking and Unlocking a Mutex
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
#include <rtems/posix/posixapi.h>

/*
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;
  Status_Control        status;

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex == NULL ) {
    return EINVAL;
  }

  status = _CORE_mutex_Surrender( &the_mutex->Mutex, &queue_context );
  return _POSIX_Get_error( status );
}
