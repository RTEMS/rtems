/**
 * @file
 *
 * @brief Sets the Thread Creation Stacksize Attribute in the attr object 
 * @ingroup POSIXAPI
 */

/*
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/stackimpl.h>

#include <pthread.h>
#include <errno.h>

int pthread_attr_setstacksize(
  pthread_attr_t  *attr,
  size_t           stacksize
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  if (stacksize < PTHREAD_MINIMUM_STACK_SIZE)
    attr->stacksize = PTHREAD_MINIMUM_STACK_SIZE;
  else
    attr->stacksize = stacksize;
  return 0;
}
