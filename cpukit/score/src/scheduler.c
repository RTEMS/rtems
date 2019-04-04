/**
 * @file
 *
 * @brief Scheduler Initialize
 * @ingroup RTEMSScoreScheduler
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
  size_t n;
  size_t i;

  n = _Scheduler_Count;

  for ( i = 0 ; i < n ; ++i ) {
    const Scheduler_Control *scheduler;
#if defined(RTEMS_SMP)
    Scheduler_Context       *context;
#endif

    scheduler = &_Scheduler_Table[ i ];
#if defined(RTEMS_SMP)
    context = _Scheduler_Get_context( scheduler );
#endif
    _ISR_lock_Initialize( &context->Lock, "Scheduler" );
    ( *scheduler->Operations.initialize )( scheduler );
  }
}
