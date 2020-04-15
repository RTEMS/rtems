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
#include <string.h>

#include <rtems/score/threadimpl.h>

int pthread_getname_np( pthread_t thread, char *name, size_t len )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  size_t            actual_len;

  _Objects_Allocator_lock();
  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    _Objects_Allocator_unlock();
    strlcpy(name, "", len);
    return ESRCH;
  }

  _ISR_lock_ISR_enable( &lock_context );
  actual_len = _Thread_Get_name( the_thread, name, len );
  _Objects_Allocator_unlock();

  if ( actual_len >= len ) {
    return ERANGE;
  }

  return 0;
}
