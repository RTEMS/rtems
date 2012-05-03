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

int _Scheduler_CBS_Get_server_id (
  rtems_id                 task_id,
  Scheduler_CBS_Server_id *server_id
)
{
  unsigned int i;
  for ( i = 0; i<_Scheduler_CBS_Maximum_servers; i++ ) {
    if ( _Scheduler_CBS_Server_list[i] &&
         _Scheduler_CBS_Server_list[i]->task_id == task_id ) {
      *server_id = i;
      return SCHEDULER_CBS_OK;
    }
  }
  return SCHEDULER_CBS_ERROR_NOSERVER;
}
