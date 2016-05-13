/**
 * @file
 *
 * @brief POSIX Thread Exit Shared Helper
 * @ingroup POSIX_THREAD Thread API Extension
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

void _POSIX_Thread_Exit(
  Thread_Control *the_thread,
  void           *value_ptr
)
{
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  cpu_self = _Thread_Dispatch_disable();
  executing = _Per_CPU_Get_executing( cpu_self );

  /*
   *  Now shut down the thread
   */
  if ( the_thread == executing ) {
    _Thread_Exit( executing, THREAD_LIFE_TERMINATING, value_ptr );
  } else {
    _Thread_Cancel( the_thread, executing, value_ptr );
  }

  _Thread_Dispatch_enable( cpu_self );
}

void pthread_exit(
  void  *value_ptr
)
{
  _POSIX_Thread_Exit( _Thread_Get_executing(), value_ptr );
  RTEMS_UNREACHABLE();
}
