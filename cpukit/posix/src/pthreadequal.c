/*
 *  16.1.7 Compare Thread IDs, p1003.1c/Draft 10, p. 153
 *
 *  NOTE:  POSIX does not define the behavior when either thread id is invalid.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
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
  int               status;
  Objects_Locations location;

  /*
   *  By default this is not a match.
   */

  status = 0;

  /*
   *  Validate the first id and return 0 if it is not valid
   */

  (void) _POSIX_Threads_Get( t1, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      break;

    case OBJECTS_LOCAL:

      /*
       *  Validate the second id and return 0 if it is not valid
       */

      (void) _POSIX_Threads_Get( t2, &location );
      switch ( location ) {
        case OBJECTS_ERROR:
        case OBJECTS_REMOTE:
          break;
        case OBJECTS_LOCAL:
          status = _Objects_Are_ids_equal( t1, t2 );
          break;
      }
      _Thread_Unnest_dispatch();
      break;
  }

  _Thread_Enable_dispatch();
  return status;
#endif
}

