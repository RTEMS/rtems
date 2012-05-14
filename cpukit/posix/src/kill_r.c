/*
 *  3.3.2 Send a Signal to a Process, P1003.1b-1993, p. 68
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

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _kill_r(
  struct _reent *ptr,
  pid_t          pid,
  int            sig
)
{
  return killinfo( pid, sig, NULL );
}
#endif
