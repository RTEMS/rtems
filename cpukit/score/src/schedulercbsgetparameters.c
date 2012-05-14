/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulercbs.h>

int _Scheduler_CBS_Get_parameters (
  Scheduler_CBS_Server_id   server_id,
  Scheduler_CBS_Parameters *params
)
{
  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  if ( !_Scheduler_CBS_Server_list[server_id] )
    return SCHEDULER_CBS_ERROR_NOSERVER;

  *params = _Scheduler_CBS_Server_list[server_id]->parameters;
  return SCHEDULER_CBS_OK;
}
