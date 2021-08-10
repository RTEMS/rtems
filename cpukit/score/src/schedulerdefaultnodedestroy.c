/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_default_Node_destroy().
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulernodeimpl.h>

void _Scheduler_default_Node_destroy(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node
)
{
  _Scheduler_Node_do_destroy( scheduler, node );
}
