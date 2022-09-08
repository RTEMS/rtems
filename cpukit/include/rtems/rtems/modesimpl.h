/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicModes
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicModes support.
 */

/*  COPYRIGHT (c) 1989-2008.
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

#ifndef _RTEMS_RTEMS_MODESIMPL_H
#define _RTEMS_RTEMS_MODESIMPL_H

#include <rtems/rtems/modes.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadcpubudget.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicModes Task Modes
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the implementation to support task modes.
 *
 * @{
 */

/**
 *  @brief Checks if mode_set says that Asynchronous Signal Processing is disabled.
 *
 *  This function returns TRUE if mode_set indicates that Asynchronous
 *  Signal Processing is disabled, and FALSE otherwise.
 */
static inline bool _Modes_Is_asr_disabled (
  rtems_mode mode_set
)
{
   return (mode_set & RTEMS_ASR_MASK) == RTEMS_NO_ASR;
}

/**
 *  @brief Checks if mode_set indicates that preemption is enabled.
 *
 *  This function returns TRUE if mode_set indicates that preemption
 *  is enabled, and FALSE otherwise.
 */
static inline bool _Modes_Is_preempt (
  rtems_mode mode_set
)
{
   return (mode_set & RTEMS_PREEMPT_MASK) == RTEMS_PREEMPT;
}

/**
 *  @brief Checks if mode_set indicates that timeslicing is enabled.
 *
 *  This function returns TRUE if mode_set indicates that timeslicing
 *  is enabled, and FALSE otherwise.
 */
static inline bool _Modes_Is_timeslice (
  rtems_mode mode_set
)
{
  return (mode_set & RTEMS_TIMESLICE_MASK) == RTEMS_TIMESLICE;
}

/**
 *  @brief Gets the interrupt level portion of the mode_set.
 *
 *  This function returns the interrupt level portion of the mode_set.
 */
static inline ISR_Level _Modes_Get_interrupt_level (
  rtems_mode mode_set
)
{
  return ( mode_set & RTEMS_INTERRUPT_MASK );
}

#if defined(RTEMS_SMP) || CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
/**
 * @brief Checks if support for the interrupt level is implemented.
 *
 * @param mode_set is the mode set which specifies the interrupt level to
 *   check.
 *
 * @return Returns true, if support for the interrupt level is implemented,
 *   otherwise returns false.
 */
static inline bool _Modes_Is_interrupt_level_supported(
  rtems_mode mode_set
)
{
  return _Modes_Get_interrupt_level( mode_set ) == 0
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
    || !_SMP_Need_inter_processor_interrupts()
#endif
    ;
}
#endif

#if defined(RTEMS_SMP)
/**
 * @brief Checks if support for the preempt mode is implemented.
 *
 * @param mode_set is the mode set which specifies the preempt mode to check.
 *
 * @param the_thread is the thread to check.
 *
 * @return Returns true, if support for the preempt mode is implemented,
 *   otherwise returns false.
 */
static inline bool _Modes_Is_preempt_mode_supported(
  rtems_mode            mode_set,
  const Thread_Control *the_thread
)
{
  return _Modes_Is_preempt( mode_set ) ||
    _Scheduler_Is_non_preempt_mode_supported(
      _Thread_Scheduler_get_home( the_thread )
    );
}
#endif

/**
 * @brief Applies the timeslice mode to the thread.
 *
 * @param mode_set is the mode set which specifies the timeslice mode for the
 *   thread.
 *
 * @param[out] the_thread is the thread to apply the timeslice mode.
 */
static inline void _Modes_Apply_timeslice_to_thread(
  rtems_mode      mode_set,
  Thread_Control *the_thread
)
{
  if ( _Modes_Is_timeslice( mode_set ) ) {
    the_thread->CPU_budget.operations = &_Thread_CPU_budget_reset_timeslice;
    the_thread->CPU_budget.available =
      rtems_configuration_get_ticks_per_timeslice();
  } else {
    the_thread->CPU_budget.operations = NULL;
  }
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
