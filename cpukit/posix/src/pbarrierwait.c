/**
 * @file
 *
 * @brief Wait at a Barrier
 * @ingroup POSIXAPI
 */

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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/barrier.h>

/**
 * This directive allows a thread to wait at a barrier.
 *
 * @param[in] barrier is the barrier id
 *
 * @retval 0 if successful
 * @retval PTHREAD_BARRIER_SERIAL_THREAD if successful
 * @retval error_code if unsuccessful
 */

int pthread_barrier_wait(
  pthread_barrier_t *barrier
)
{
  POSIX_Barrier_Control   *the_barrier = NULL;
  Objects_Locations        location;

  if ( !barrier )
    return EINVAL;

  the_barrier = _POSIX_Barrier_Get( barrier, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_barrier_Wait(
        &the_barrier->Barrier,
        the_barrier->Object.id,
        true,
        0,
        NULL
      );
      _Objects_Put( &the_barrier->Object );
      return _POSIX_Barrier_Translate_core_barrier_return_code(
                _Thread_Executing->Wait.return_code );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
