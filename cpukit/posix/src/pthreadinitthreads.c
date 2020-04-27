/**
 * @file
 *
 * @ingroup POSIX_PTHREAD
 *
 * @brief POSIX Threads Initialize User Threads Body
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/pthread.h>

#include <pthread.h>

#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/interr.h>

void _POSIX_Threads_Initialize_user_thread( void )
{
  int                                       eno;
  const posix_initialization_threads_table *user_thread;
  pthread_t                                 thread_id;
  pthread_attr_t                            attr;

  user_thread = &_POSIX_Threads_User_thread_table;

  /*
   *  Be careful .. if the default attribute set changes, this may need to.
   *
   *  Setting the attributes explicitly is critical, since we don't want
   *  to inherit the idle tasks attributes.
   */

  /*
   * There is no way for these calls to fail in this situation.
   */
  eno = pthread_attr_init( &attr );
  _Assert( eno == 0 );
  eno = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  _Assert( eno == 0 );
  eno = pthread_attr_setstacksize( &attr, user_thread->stack_size );
  _Assert( eno == 0 );

  eno = pthread_create(
    &thread_id,
    &attr,
    user_thread->thread_entry,
    NULL
  );
  if ( eno != 0 ) {
    _Internal_error( INTERNAL_ERROR_POSIX_INIT_THREAD_CREATE_FAILED );
  }

  if ( _Thread_Global_constructor == 0 ) {
    _Thread_Global_constructor = thread_id;
  }
}
