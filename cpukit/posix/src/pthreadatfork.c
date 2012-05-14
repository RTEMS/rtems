/*
 *  pthread_atfork() - POSIX 1003.1b 3.1.3
 *
 *  3.1.3 Register Fork Handlers, P1003.1c/Draft 10, P1003.1c/Draft 10, p. 27
 *
 *  RTEMS does not support processes, so we fall under this and do not
 *  provide this routine:
 *
 *  "Either the implementation shall support the pthread_atfork() function
 *   as described above or the pthread_atfork() funciton shall not be
 *   provided."
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/seterr.h>

int pthread_atfork(
  void (*prepare)(void) __attribute__((unused)),
  void (*parent)(void) __attribute__((unused)),
  void (*child)(void) __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
