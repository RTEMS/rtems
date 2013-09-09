/**
 * @file
 *
 * @brief POSIX Threads Initialize User Threads Body
 * @ingroup POSIX_PTHREAD
 */

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
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/config.h>
#include <rtems/posix/time.h>

void _POSIX_Threads_Initialize_user_threads_body(void)
{
  int                                 eno;
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
    eno = pthread_attr_init( &attr );
    _Assert( eno == 0 );
    eno = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    _Assert( eno == 0 );
    eno = pthread_attr_setstacksize(&attr, user_threads[ index ].stack_size);
    _Assert( eno == 0 );

    eno = pthread_create(
      &thread_id,
      &attr,
      user_threads[ index ].thread_entry,
      NULL
    );
    if ( eno )
      _POSIX_Fatal_error( POSIX_FD_PTHREAD, eno );
  }
}
