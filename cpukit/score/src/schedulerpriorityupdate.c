/**
 * @file
 *
 * @brief Update Scheduler Priority 
 * @ingroup ScoreScheduler
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

void _Scheduler_priority_Update(
  Scheduler_Control *base,
  Thread_Control    *the_thread
)
{
  Scheduler_priority_Control *self =
    _Scheduler_priority_Self_from_base( base );

  _Scheduler_priority_Update_body(
    the_thread,
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}
