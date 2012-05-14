/*
 *  16.1.1 Thread Creation Attributes
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_ATTR_SETSTACK
#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>

int pthread_attr_setstack(
  pthread_attr_t  *attr,
  void            *stackaddr,
  size_t           stacksize
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  if (stacksize < PTHREAD_MINIMUM_STACK_SIZE)
    attr->stacksize = PTHREAD_MINIMUM_STACK_SIZE;
  else
    attr->stacksize = stacksize;

  attr->stackaddr = stackaddr;
  return 0;
}
#endif
