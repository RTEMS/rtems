/*
 *  16.1.8 Dynamic Package Initialization, P1003.1c/Draft 10, p. 154
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>

int pthread_once(
  pthread_once_t  *once_control,
  void           (*init_routine)(void)
)
{
  if ( !once_control || !init_routine )
    return EINVAL;

  _Thread_Disable_dispatch();

  if ( !once_control->init_executed ) {
    once_control->is_initialized = TRUE;
    once_control->init_executed = TRUE;
    (*init_routine)();
  }

  _Thread_Enable_dispatch();
  return 0;
}

