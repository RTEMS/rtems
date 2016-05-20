/**
 * @file
 *
 * @brief Pthread Get Attribute
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_getattr_np(
  pthread_t       thread,
  pthread_attr_t *attr
)
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  POSIX_API_Control *api;

  if ( attr == NULL ) {
    return EINVAL;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];
  _POSIX_Threads_Copy_attributes( attr, &api->Attributes);

  if ( _Thread_Is_joinable( the_thread ) ) {
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
  } else {
    attr->detachstate = PTHREAD_CREATE_DETACHED;
  }

  _Thread_State_release( the_thread, &lock_context );
  return 0;
}
