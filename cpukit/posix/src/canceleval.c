/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthread.h>

void _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch(
  Thread_Control    *the_thread
)
{
  POSIX_API_Control *thread_support;

  thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];

  if ( thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE &&
       thread_support->cancelability_type == PTHREAD_CANCEL_ASYNCHRONOUS &&
       thread_support->cancelation_requested ) {
    _Thread_Unnest_dispatch();
    _POSIX_Thread_Exit( the_thread, PTHREAD_CANCELED );
  } else
    _Thread_Enable_dispatch();

}
