/**
 *  @file
 *
 *  @brief Signal a Condition 
 *  @ingroup POSIXAPI
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
#include <rtems/score/watchdog.h>
#include <rtems/posix/condimpl.h>
#include <rtems/posix/time.h>
#include <rtems/posix/muteximpl.h>

/**
 *  11.4.3 Broadcasting and Signaling a Condition, P1003.1c/Draft 10, p. 101
 */
int pthread_cond_signal(
  pthread_cond_t   *cond
)
{
  return _POSIX_Condition_variables_Signal_support( cond, false );
}
