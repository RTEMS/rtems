/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/apiext.h>
#include <rtems/score/stack.h>
#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Threads_Initialize_user_threads_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Threads_Initialize_user_threads_body(void)
{
  int                                 status;
  uint32_t                            index;
  uint32_t                            maximum;
  posix_initialization_threads_table *user_threads;
  pthread_t                           thread_id;
  pthread_attr_t                      attr;

  user_threads = Configuration_POSIX_API.User_initialization_threads_table;
  maximum      = Configuration_POSIX_API.number_of_initialization_threads;

  if ( !user_threads || maximum == 0 )
    return;

  /*
   *  Be careful .. if the default attribute set changes, this may need to.
   *
   *  Setting the attributes explicitly is critical, since we don't want
   *  to inherit the idle tasks attributes.
   */

  for ( index=0 ; index < maximum ; index++ ) {
    /*
     * There is no way for these calls to fail in this situation.
     */
    (void) pthread_attr_init( &attr );
    (void) pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    (void) pthread_attr_setstacksize(&attr, user_threads[ index ].stack_size);

    status = pthread_create(
      &thread_id,
      &attr,
      user_threads[ index ].thread_entry,
      NULL
    );
    if ( status )
      _Internal_error_Occurred( INTERNAL_ERROR_POSIX_API, true, status );
  }
}

