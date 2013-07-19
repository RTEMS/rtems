/**
 * @file
 *
 * @brief Send a Signal to a Process
 * @ingroup POSIXAPI
 */

/*
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
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>

int kill(
  pid_t pid,
  int   sig
)
{
  return killinfo( pid, sig, NULL );
}

