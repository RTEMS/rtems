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

void _Scheduler_priority_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );

  _Priority_bit_map_Initialize( &context->Bit_map );
  _Scheduler_priority_Ready_queue_initialize( &context->Ready[ 0 ] );
}
