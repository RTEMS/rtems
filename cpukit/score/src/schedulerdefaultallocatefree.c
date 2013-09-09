/**
 * @file
 *
 * @brief Scheduler Default Allocate and Release Operation
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>

void *_Scheduler_default_Allocate(
  Thread_Control *thread
)
{
  ( void ) thread;

  return ( void * )-1; /* maybe pick an appropriate poison value */
}

void _Scheduler_default_Free(
  Thread_Control *thread
)
{
  ( void ) thread;
}
