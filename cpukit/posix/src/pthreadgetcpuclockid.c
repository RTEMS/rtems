/*
 *  20.1.6 Accessing a Thread CPU-time Clock, P1003.4b/Draft 8, p. 58
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>

int pthread_getcpuclockid(
  pthread_t    pid,
  clockid_t   *clock_id
)
{
  return POSIX_NOT_IMPLEMENTED();
}

