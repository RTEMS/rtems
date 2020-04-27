/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Send a Signal to a Process
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>

int kill(
  pid_t pid,
  int   sig
)
{
  return _POSIX_signals_Send( pid, sig, NULL );
}

