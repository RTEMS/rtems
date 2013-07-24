/**
 * @file
 *
 * @brief Yield Processor
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

#include <sched.h>

#include <rtems/score/percpu.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>

int sched_yield( void )
{
  _Thread_Disable_dispatch();
    _Scheduler_Yield( _Thread_Executing );
  _Thread_Enable_dispatch();
  return 0;
}
