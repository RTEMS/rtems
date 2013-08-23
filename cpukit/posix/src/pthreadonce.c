/**
 * @file
 *
 * @brief Call to function by Thread will call init_routine with no Arguments
 * @ingroup POSIXAPI
 */

/*
 *  16.1.8 Dynamic Package Initialization, P1003.1c/Draft 10, p. 154
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/posix/onceimpl.h>

#define PTHREAD_ONCE_INIT_NOT_RUN  0
#define PTHREAD_ONCE_INIT_RUNNING  1
#define PTHREAD_ONCE_INIT_COMPLETE 2

int pthread_once(
  pthread_once_t  *once_control,
  void           (*init_routine)(void)
)
{
  int r = 0;

  if ( !once_control || !init_routine )
    return EINVAL;

  if ( once_control->is_initialized != 1 )
    return EINVAL;

  if ( once_control->init_executed != PTHREAD_ONCE_INIT_COMPLETE ) {
    r = pthread_mutex_lock( &_POSIX_Once_Lock );
    if ( r == 0 ) {
      int rr;

      /*
       * Getting to here means the once_control is locked so we have:
       *  1. The init has not run and the state is PTHREAD_ONCE_INIT_NOT_RUN.
       *  2. The init has finished and the state is PTHREAD_ONCE_INIT_RUN.
       *  3. The init is being run by this thread and the state
       *     PTHREAD_ONCE_INIT_RUNNING so we are nesting. This is an error.
       */

      switch ( once_control->init_executed ) {
        case PTHREAD_ONCE_INIT_NOT_RUN:
          once_control->init_executed = PTHREAD_ONCE_INIT_RUNNING;
          (*init_routine)();
          once_control->init_executed = PTHREAD_ONCE_INIT_COMPLETE;
          break;
        case PTHREAD_ONCE_INIT_RUNNING:
          r = EINVAL;
          break;
        default:
          break;
      }
      rr = pthread_mutex_unlock( &_POSIX_Once_Lock );
      if ( r == 0 )
        r = rr;
    }
  }

  return r;
}
