/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the definition of
 *   ::_Thread_CPU_budget_reset_timeslice and the implementation of
 *   _Thread_CPU_budget_consume_and_yield() and
 *   _Thread_CPU_budget_set_to_ticks_per_timeslice().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 2010 Gedare Bloom  <gedare@rtems.org>
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

#include <rtems/score/threadcpubudget.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/watchdogticks.h>

void _Thread_CPU_budget_consume_and_yield( Thread_Control *the_thread )
{
  uint32_t budget_available;

  if ( !the_thread->is_preemptible ) {
    return;
  }

  if ( !_States_Is_ready( the_thread->current_state ) ) {
    return;
  }

  budget_available = the_thread->CPU_budget.available;

  if ( budget_available == 1 ) {
    the_thread->CPU_budget.available = _Watchdog_Ticks_per_timeslice;

    /*
     *  A yield performs the ready chain mechanics needed when
     *  resetting a timeslice.  If no other thread's are ready
     *  at the priority of the currently executing thread, then the
     *  executing thread's timeslice is reset.  Otherwise, the
     *  currently executing thread is placed at the rear of the
     *  FIFO for this priority and a new heir is selected.
     */
    _Thread_Yield( the_thread );
  } else {
    the_thread->CPU_budget.available = budget_available - 1;
  }
}

void _Thread_CPU_budget_set_to_ticks_per_timeslice(
  Thread_Control *the_thread
)
{
  the_thread->CPU_budget.available = _Watchdog_Ticks_per_timeslice;
}

const Thread_CPU_budget_operations _Thread_CPU_budget_reset_timeslice = {
  .at_tick = _Thread_CPU_budget_consume_and_yield,
  .at_context_switch = _Thread_CPU_budget_set_to_ticks_per_timeslice,
  .initialize = _Thread_CPU_budget_set_to_ticks_per_timeslice
};
