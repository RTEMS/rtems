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
  pthread_t       id,
  pthread_attr_t *attr
)
{
  Objects_Locations        location;
  POSIX_API_Control       *api;
  Thread_Control          *the_thread;

  if ( !attr )
    return EINVAL;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];
      _POSIX_Threads_Copy_attributes( attr, &api->Attributes);
      _Objects_Put( &the_thread->Object );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
