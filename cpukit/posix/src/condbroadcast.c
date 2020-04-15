/**
 *  @file
 *
 *  @brief Broadcast a Condition
 *  @ingroup POSIXAPI
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

#include <rtems/posix/condimpl.h>

/**
 *  11.4.3 Broadcasting and Signaling a Condition, P1003.1c/Draft 10, p. 101
 */
int pthread_cond_broadcast(
  pthread_cond_t   *cond
)
{
  return _POSIX_Condition_variables_Signal_support( cond, true );
}
