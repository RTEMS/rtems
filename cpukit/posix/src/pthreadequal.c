/**
 * @file
 *
 * @brief Compare Thread IDs
 * @ingroup POSIXAPI
 */

/*
 *  NOTE:  POSIX does not define the behavior when either thread id is invalid.
 *
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
#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/thread.h>

int pthread_equal(
  pthread_t  t1,
  pthread_t  t2
)
{
  /*
   *  If the system is configured for debug, then we will do everything we
   *  can to insure that both ids are valid.  Otherwise, we will do the
   *  cheapest possible thing to determine if they are equal.
   */

#ifndef RTEMS_DEBUG
  return _Objects_Are_ids_equal( t1, t2 );
#else
  int                status;
  Objects_Locations  location;
  Thread_Control    *thread_1;
  Thread_Control    *thread_2;

  /*
   *  By default this is not a match.
   */

  status = 0;

  /*
   *  Validate the first id and return 0 if it is not valid
   */

  thread_1 = _Thread_Get( t1, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      /*
       *  Validate the second id and return 0 if it is not valid
       */

      thread_2 = _Thread_Get( t2, &location );
      switch ( location ) {

        case OBJECTS_LOCAL:
          status = _Objects_Are_ids_equal( t1, t2 );
          _Objects_Put_without_thread_dispatch( &thread_2->Object );
          _Objects_Put( &thread_1->Object );
          break;

        case OBJECTS_ERROR:
#if defined(RTEMS_MULTIPROCESSING)
        case OBJECTS_REMOTE:
#endif
          /* t1 must have been valid so exit the critical section */
          _Objects_Put( &thread_1->Object );
          /* return status == 0 */
          break;
      }
      break;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      /* return status == 0 */
      break;
  }

  return status;
#endif
}
