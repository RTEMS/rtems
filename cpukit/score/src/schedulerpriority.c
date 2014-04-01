/*
 *  @file
 *
 *  @brief Initialize Scheduler Priority
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/wkspace.h>

void _Scheduler_priority_Initialize(void)
{
  Scheduler_priority_Control *self = _Workspace_Allocate_or_fatal_error(
    sizeof( *self ) + PRIORITY_MAXIMUM * sizeof( Chain_Control )
  );

  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize( &self->Ready[ 0 ] );

  _Scheduler.information = self;
}
