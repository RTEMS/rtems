/**
 * @file
 *
 * @brief Scheduler CBS Release Job
 *
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbsimpl.h>

void _Scheduler_CBS_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  uint64_t                 deadline
)
{
  Scheduler_CBS_Node   *node;
  Scheduler_CBS_Server *serv_info;

  _Scheduler_EDF_Release_job( scheduler, the_thread, deadline );

  node = _Scheduler_CBS_Thread_get_node( the_thread );
  serv_info = node->cbs_server;

  /* Budget replenishment for the next job. */
  if ( serv_info != NULL ) {
    the_thread->cpu_time_budget = serv_info->parameters.budget;
  }
}
