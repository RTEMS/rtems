/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProfiling
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreProfiling.
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_PROFILING
#define _RTEMS_SCORE_PROFILING

#include <rtems/score/percpu.h>
#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreProfiling Profiling Support
 * 
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support profiling.
 *
 * @{
 */

/**
 * @brief Disables the thread dispatch if the previous thread dispatch
 *      disable level is zero.
 *
 * @param[out] cpu The cpu control.
 * @param previous_thread_dispatch_disable_level The dispatch disable
 *      level of the previous thread.
 */
static inline void _Profiling_Thread_dispatch_disable(
  Per_CPU_Control *cpu,
  uint32_t previous_thread_dispatch_disable_level
)
{
#if defined( RTEMS_PROFILING )
  if ( previous_thread_dispatch_disable_level == 0 ) {
    Per_CPU_Stats *stats = &cpu->Stats;

    stats->thread_dispatch_disabled_instant = _CPU_Counter_read();
    ++stats->thread_dispatch_disabled_count;
  }
#else
  (void) cpu;
  (void) previous_thread_dispatch_disable_level;
#endif
}

/**
 * @brief Disables the thread dispatch.
 *
 * Only if the previous thread dispatch disable level is zero.  This
 * method also takes into account the lock_context.
 *
 * @param[out] cpu The cpu control.
 * @param previous_thread_dispatch_disable_level The dispatch disable
 *      level of the previous thread.
 * @param lock_context The lock context.
 */
static inline void _Profiling_Thread_dispatch_disable_critical(
  Per_CPU_Control        *cpu,
  uint32_t                previous_thread_dispatch_disable_level,
  const ISR_lock_Context *lock_context
)
{
#if defined( RTEMS_PROFILING )
  if ( previous_thread_dispatch_disable_level == 0 ) {
    Per_CPU_Stats *stats = &cpu->Stats;

    stats->thread_dispatch_disabled_instant = lock_context->ISR_disable_instant;
    ++stats->thread_dispatch_disabled_count;
  }
#else
  (void) cpu;
  (void) previous_thread_dispatch_disable_level;
  (void) lock_context;
#endif
}

/**
 * @brief Enables the thread dispatch.
 *
 * Only if the @a new_thread_dispatch_disable_level is 0.
 *
 * @param[out] cpu The cpu control.
 * @param new_thread_dispatch_disable_level The dispatch disable level
 *      of the new thread.
 */
static inline void _Profiling_Thread_dispatch_enable(
  Per_CPU_Control *cpu,
  uint32_t new_thread_dispatch_disable_level
)
{
#if defined( RTEMS_PROFILING )
  if ( new_thread_dispatch_disable_level == 0 ) {
    Per_CPU_Stats *stats = &cpu->Stats;
    CPU_Counter_ticks now = _CPU_Counter_read();
    CPU_Counter_ticks delta = now - stats->thread_dispatch_disabled_instant;

    stats->total_thread_dispatch_disabled_time += delta;

    if ( stats->max_thread_dispatch_disabled_time < delta ) {
      stats->max_thread_dispatch_disabled_time = delta;
    }
  }
#else
  (void) cpu;
  (void) new_thread_dispatch_disable_level;
#endif
}

/**
 * @brief Updates the maximum interrupt delay.
 *
 * @param[out] cpu The cpu control.
 * @param interrupt_delay The new interrupt delay.
 */
static inline void _Profiling_Update_max_interrupt_delay(
  Per_CPU_Control *cpu,
  CPU_Counter_ticks interrupt_delay
)
{
#if defined( RTEMS_PROFILING )
  Per_CPU_Stats *stats = &cpu->Stats;

  if ( stats->max_interrupt_delay < interrupt_delay ) {
    stats->max_interrupt_delay = interrupt_delay;
  }
#else
  (void) cpu;
  (void) interrupt_delay;
#endif
}

/**
 * @brief Updates the interrupt profiling statistics.
 *
 * Must be called with the interrupt stack and before the thread dispatch
 * disable level is decremented.
 *
 * @param cpu The cpu control.
 * @param interrupt_entry_instant The instant that the interrupt occurred.
 * @param interrupt_exit_instant The instant in which the interrupt was exited.
 */
void _Profiling_Outer_most_interrupt_entry_and_exit(
  Per_CPU_Control *cpu,
  CPU_Counter_ticks interrupt_entry_instant,
  CPU_Counter_ticks interrupt_exit_instant
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PROFILING */
