/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Create_server().
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbs.h>
#include <rtems/score/scheduleredfimpl.h>

int _Scheduler_CBS_Create_server (
  Scheduler_CBS_Parameters     *params,
  Scheduler_CBS_Budget_overrun  budget_overrun_callback,
  rtems_id                     *server_id
)
{
  unsigned int i;
  Scheduler_CBS_Server *the_server;

  if ( params->budget <= 0 ||
       params->deadline <= 0 ||
       params->budget >= SCHEDULER_EDF_PRIO_MSB ||
       params->deadline >= SCHEDULER_EDF_PRIO_MSB )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  for ( i = 0; i<_Scheduler_CBS_Maximum_servers; i++ ) {
    if ( !_Scheduler_CBS_Server_list[i].initialized )
      break;
  }

  if ( i == _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_FULL;

  *server_id = i;
  the_server = &_Scheduler_CBS_Server_list[*server_id];
  the_server->parameters = *params;
  the_server->task_id = -1;
  the_server->cbs_budget_overrun = budget_overrun_callback;
  the_server->initialized = true;
  return SCHEDULER_CBS_OK;
}
