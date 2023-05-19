/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Get_CPU_time_used_after_last_reset().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

Timestamp_Control _Thread_Get_CPU_time_used_after_last_reset(
  Thread_Control *the_thread
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         state_lock_context;
  ISR_lock_Context         scheduler_lock_context;
  Timestamp_Control        cpu_time_used;
  Timestamp_Control        cpu_time_used_at_last_reset;

  _Thread_State_acquire( the_thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  cpu_time_used = _Thread_Get_CPU_time_used_locked( the_thread );
  cpu_time_used_at_last_reset = the_thread->cpu_time_used_at_last_reset;

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( the_thread, &state_lock_context );

  _Timestamp_Subtract(
    &cpu_time_used_at_last_reset,
    &cpu_time_used,
    &cpu_time_used
  );

  return cpu_time_used;
}
