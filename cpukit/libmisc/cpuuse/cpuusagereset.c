/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplCPUUsageReporting
 *
 * @brief This source file contains the definition of
 *   rtems_cpu_usage_reset().
 */

/*
 *  COPYRIGHT (c) 1989-2009
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/cpuuse.h>
#include <rtems/rtems/scheduler.h>
#include <rtems/score/percpu.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/watchdogimpl.h>

#include "cpuuseimpl.h"

static bool CPU_usage_Per_thread_handler(
  Thread_Control *the_thread,
  void           *arg
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         state_lock_context;
  ISR_lock_Context         scheduler_lock_context;

  _Thread_State_acquire( the_thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  the_thread->cpu_time_used_at_last_reset =
    _Thread_Get_CPU_time_used_locked( the_thread );

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( the_thread, &state_lock_context );
  return false;
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  uint32_t cpu_count;
  uint32_t cpu_index;

  _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );

  cpu_count = rtems_scheduler_get_processor_maximum();
  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    cpu->cpu_usage_timestamp = CPU_usage_Uptime_at_last_reset;
  }

  rtems_task_iterate(CPU_usage_Per_thread_handler, NULL);
}
