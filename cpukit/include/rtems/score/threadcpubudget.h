/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This header file provides interfaces used to implement the CPU budget
 *   management of threads.
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

#ifndef _RTEMS_SCORE_THREADCPUBUDGET_H
#define _RTEMS_SCORE_THREADCPUBUDGET_H

#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreThread
 *
 * @{
 */

/**
 * @brief Does nothing.
 *
 * @param the_thread is an unused parameter.
 */
void _Thread_CPU_budget_do_nothing( Thread_Control *the_thread );

/**
 * @brief Sets the available CPU budget of the thread to the configured clock
 *   ticks per timeslice.
 *
 * @param the_thread is the thread to set the available CPU budget.
 */
void _Thread_CPU_budget_set_to_ticks_per_timeslice(
  Thread_Control *the_thread
);

/**
 * @brief Consumes one time quantum of the available CPU budget of the thread
 *   and yields the thread if the available CPU budget is fully consumed.
 *
 * While the thread enabled the non-preemptive mode or is not ready, no time
 * quantum is consumed.
 *
 * @param the_thread is the thread to operate on.
 */
void _Thread_CPU_budget_consume_and_yield( Thread_Control *the_thread );

/**
 * @brief These CPU budget operations allocate timeslices to the thread.
 *
 * The timeslice is not reset at a context switch to the thread.  Once a
 * timeslice is consumed, the thread yields.
 */
extern const Thread_CPU_budget_operations _Thread_CPU_budget_exhaust_timeslice;

/**
 * @brief These CPU budget operations allocate timeslices to the thread.
 *
 * The timeslice is reset at a context switch to the thread.  Once a timeslice
 * is consumed, the thread yields.
 */
extern const Thread_CPU_budget_operations _Thread_CPU_budget_reset_timeslice;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_THREADCPUBUDGET_H */
