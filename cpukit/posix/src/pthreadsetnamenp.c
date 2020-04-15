/*
 * Copyright (c) 2017 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/score/threadimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_setname_np( pthread_t thread, const char *name )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Status_Control    status;

  _Objects_Allocator_lock();
  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    _Objects_Allocator_unlock();
    return ESRCH;
  }

  _ISR_lock_ISR_enable( &lock_context );
  status = _Thread_Set_name( the_thread, name );
  _Objects_Allocator_unlock();
  return _POSIX_Get_error( status );
}
