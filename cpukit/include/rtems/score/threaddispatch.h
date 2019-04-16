/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief Constants and Structures Related with Thread Dispatch
 */

/*
 * COPYRIGHT (c) 1989-2009.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADDISPATCH_H
#define _RTEMS_SCORE_THREADDISPATCH_H

#include <rtems/score/percpu.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/profiling.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreThread
 *
 * @{
 */

#if defined(RTEMS_SMP) || ( CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE )
/**
 * @brief Enables a robust thread dispatch.
 *
 * On each change of the thread dispatch disable level from one to zero the
 * interrupt status is checked.  In case interrupts are disabled and SMP is
 * enabled or the CPU port needs it, then the system terminates with the fatal
 * internal error INTERNAL_ERROR_BAD_THREAD_DISPATCH_ENVIRONMENT.
 */
#define RTEMS_SCORE_ROBUST_THREAD_DISPATCH
#endif

/**
 * @brief Indicates if the executing thread is inside a thread dispatch
 * critical section.
 *
 * @retval true Thread dispatching is enabled.
 * @retval false The executing thread is inside a thread dispatch critical
 * section and dispatching is not allowed.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Dispatch_is_enabled(void)
{
  bool enabled;

#if defined(RTEMS_SMP)
  ISR_Level level;

  _ISR_Local_disable( level );
#endif

  enabled = _Thread_Dispatch_disable_level == 0;

#if defined(RTEMS_SMP)
  _ISR_Local_enable( level );
#endif

  return enabled;
}

/**
 * @brief Gets thread dispatch disable level.
 *
 * @return The value of the thread dispatch level.
 */
RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_get_disable_level(void)
{
  return _Thread_Dispatch_disable_level;
}

/**
 * @brief Thread dispatch initialization.
 *
 * This routine initializes the thread dispatching subsystem.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_initialization( void )
{
  _Thread_Dispatch_disable_level = 1;
}

/**
 * @brief Performs a thread dispatch if necessary.
 *
 * This routine is responsible for transferring control of the processor from
 * the executing thread to the heir thread.  Once the heir is running an
 * attempt is made to run the pending post-switch thread actions.
 *
 * As part of this process, it is responsible for the following actions
 *   - update timing information of the executing thread,
 *   - save the context of the executing thread,
 *   - invokation of the thread switch user extensions,
 *   - restore the context of the heir thread, and
 *   - run of pending post-switch thread actions of the resulting executing
 *     thread.
 *
 * On entry the thread dispatch level must be equal to zero.
 */
void _Thread_Dispatch( void );

/**
 * @brief Directly do a thread dispatch.
 *
 * Must be called with a thread dispatch disable level of one, otherwise the
 * INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL will occur.  This function
 * is useful for operations which synchronously block, e.g. self restart, self
 * deletion, yield, sleep.
 *
 * @param cpu_self The current processor.
 *
 * @see _Thread_Dispatch().
 */
void _Thread_Dispatch_direct( Per_CPU_Control *cpu_self );

/**
 * @brief Performs a thread dispatch on the current processor.
 *
 * On entry the thread dispatch disable level must be equal to one and
 * interrupts must be disabled.
 *
 * This function assumes that a thread dispatch is necessary.
 *
 * @param cpu_self The current processor.
 * @param level The previous interrupt level.
 *
 * @see _Thread_Dispatch().
 */
void _Thread_Do_dispatch( Per_CPU_Control *cpu_self, ISR_Level level );

/**
 * @brief Disables thread dispatching inside a critical section (interrupts
 * disabled) with the current processor.
 *
 * @param cpu_self The current processor.
 * @param lock_context The lock context of the corresponding
 * _ISR_lock_ISR_disable() that started the critical section.
 *
 * @return The current processor.
 */
RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Dispatch_disable_with_CPU(
  Per_CPU_Control        *cpu_self,
  const ISR_lock_Context *lock_context
)
{
  uint32_t disable_level;

  disable_level = cpu_self->thread_dispatch_disable_level;
  _Profiling_Thread_dispatch_disable_critical(
    cpu_self,
    disable_level,
    lock_context
  );
  cpu_self->thread_dispatch_disable_level = disable_level + 1;

  return cpu_self;
}

/**
 * @brief Disables thread dispatching inside a critical section (interrupts
 * disabled).
 *
 * @param lock_context The lock context of the corresponding
 * _ISR_lock_ISR_disable() that started the critical section.
 *
 * @return The current processor.
 */
RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Dispatch_disable_critical(
  const ISR_lock_Context *lock_context
)
{
  return _Thread_Dispatch_disable_with_CPU( _Per_CPU_Get(), lock_context );
}

/**
 * @brief Disables thread dispatching.
 *
 * @return The current processor.
 */
RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Dispatch_disable( void )
{
  Per_CPU_Control  *cpu_self;
  ISR_lock_Context  lock_context;

#if defined( RTEMS_SMP ) || defined( RTEMS_PROFILING )
  _ISR_lock_ISR_disable( &lock_context );
#endif

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );

#if defined( RTEMS_SMP ) || defined( RTEMS_PROFILING )
  _ISR_lock_ISR_enable( &lock_context );
#endif

  return cpu_self;
}

/**
 * @brief Enables thread dispatching.
 *
 * May perform a thread dispatch if necessary as a side-effect.
 *
 * @param[in, out] cpu_self The current processor.
 */
void _Thread_Dispatch_enable( Per_CPU_Control *cpu_self );

/**
 * @brief Unnests thread dispatching.
 *
 * @param[in, out] cpu_self The current processor.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_unnest( Per_CPU_Control *cpu_self )
{
  _Assert( cpu_self->thread_dispatch_disable_level > 0 );
  --cpu_self->thread_dispatch_disable_level;
}

/**
 * @brief Requests a thread dispatch on the target processor.
 *
 * @param[in, out] cpu_self The current processor.
 * @param[in, out] cpu_target The target processor to request a thread dispatch.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_request(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_target
)
{
#if defined( RTEMS_SMP )
  if ( cpu_self == cpu_target ) {
    cpu_self->dispatch_necessary = true;
  } else {
    _Atomic_Fetch_or_ulong( &cpu_target->message, 0, ATOMIC_ORDER_RELEASE );
    _CPU_SMP_Send_interrupt( _Per_CPU_Get_index( cpu_target ) );
  }
#else
 cpu_self->dispatch_necessary = true;
 (void) cpu_target;
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_THREADDISPATCH_H */
