/**
 *  @file
 *
 *  @brief Lock a Semaphore
 *  @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

int sem_wait(
  sem_t *sem
)
{
  return _POSIX_Semaphore_Wait_support( sem, true, THREAD_QUEUE_WAIT_FOREVER );
}
