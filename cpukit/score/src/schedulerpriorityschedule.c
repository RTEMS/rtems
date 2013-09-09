/**
 * @file
 *
 * @brief Priority Scheduler Schedule Method
 * @ingroup ScoreScheduler
 */
 
/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Schedule( Thread_Control *thread )
{
  _Scheduler_priority_Schedule_body( thread, false );
}
