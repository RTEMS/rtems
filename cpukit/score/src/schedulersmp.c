/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_Request_ask_for_help().
 */

/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersmpimpl.h>

void _Scheduler_SMP_Remove_ask_for_help_from_processor(
  Thread_Control  *thread,
  Per_CPU_Control *cpu
)
{
  ISR_lock_Context lock_context;

  _Assert( _ISR_lock_Is_owner( &thread->Scheduler.Lock ) );

  _Per_CPU_Acquire( cpu, &lock_context );

  if ( thread->Scheduler.ask_for_help_cpu == cpu ) {
    _Chain_Extract_unprotected( &thread->Scheduler.Help_node );
    thread->Scheduler.ask_for_help_cpu = NULL;
  }

  _Per_CPU_Release( cpu, &lock_context );
}
