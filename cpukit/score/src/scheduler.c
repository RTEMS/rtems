/**
 * @file
 *
 * @brief Scheduler Initialize
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

#include <rtems/score/schedulerimpl.h>

void _Scheduler_Handler_initialization(void)
{
  size_t n = _Scheduler_Count;
  size_t i;

  for ( i = 0 ; i < n ; ++i ) {
    const Scheduler_Control *scheduler = &_Scheduler_Table[ i ];

    ( *scheduler->Operations.initialize )( scheduler );
  }
}
