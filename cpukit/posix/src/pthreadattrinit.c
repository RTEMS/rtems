/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Thread Attributes Creation
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/pthreadattrimpl.h>

/**
 * 16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */
int pthread_attr_init(
  pthread_attr_t  *attr
)
{
  if ( !attr )
    return EINVAL;

  _POSIX_Threads_Initialize_attributes( attr );

  return 0;
}
