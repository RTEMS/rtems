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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MODESIMPL_H
#define _RTEMS_RTEMS_MODESIMPL_H

#include <rtems/rtems/modes.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threadimpl.h>
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
RTEMS_INLINE_ROUTINE bool _Modes_Is_asr_disabled (
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
RTEMS_INLINE_ROUTINE bool _Modes_Is_preempt (
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
RTEMS_INLINE_ROUTINE bool _Modes_Is_timeslice (
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
RTEMS_INLINE_ROUTINE ISR_Level _Modes_Get_interrupt_level (
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
RTEMS_INLINE_ROUTINE bool _Modes_Is_interrupt_level_supported(
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
RTEMS_INLINE_ROUTINE bool _Modes_Is_preempt_mode_supported(
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
RTEMS_INLINE_ROUTINE void _Modes_Apply_timeslice_to_thread(
  rtems_mode      mode_set,
  Thread_Control *the_thread
)
{
  if ( _Modes_Is_timeslice( mode_set ) ) {
    the_thread->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
    the_thread->cpu_time_budget = rtems_configuration_get_ticks_per_timeslice();
  } else {
    the_thread->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  }
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
