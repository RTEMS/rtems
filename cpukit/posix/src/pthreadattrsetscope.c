/*
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>

int pthread_attr_setscope(
  pthread_attr_t  *attr,
  int              contentionscope
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( contentionscope ) {
    case PTHREAD_SCOPE_PROCESS:
      attr->contentionscope = contentionscope;
      return 0;

    case PTHREAD_SCOPE_SYSTEM:
      return ENOTSUP;

    default:
      return EINVAL;
  }
}
