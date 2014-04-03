/**
 * @file
 *
 * @brief Scheduler Simple Functions
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/wkspace.h>

void _Scheduler_simple_Initialize ( void )
{
  Scheduler_simple_Control *scheduler =
    _Workspace_Allocate_or_fatal_error( sizeof( *scheduler ) );

  _Chain_Initialize_empty( &scheduler->Ready );

  _Scheduler.information = scheduler;
}
