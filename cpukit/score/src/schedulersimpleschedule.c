/**
 * @file
 *
 * @brief Simple Schedule Determine Heir
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>

void _Scheduler_simple_Schedule(void)
{
  _Thread_Heir = (Thread_Control *) _Chain_First(
    (Chain_Control *) _Scheduler.information
  );
}
