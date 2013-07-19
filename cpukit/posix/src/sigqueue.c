/**
 * @file
 *
 * @brief Queue a Signal to a Process
 * @ingroup POSIXAPI
 */

/*
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
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>

int sigqueue(
  pid_t               pid,
  int                 signo,
  const union sigval  value
)
{
  return killinfo( pid, signo, &value );
}
