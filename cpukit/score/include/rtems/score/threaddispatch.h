/**
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

#if defined(RTEMS_HEAVY_STACK_DEBUG) || \
    defined(RTEMS_HEAVY_MALLOC_DEBUG)
  #define __THREAD_DO_NOT_INLINE_DISABLE_DISPATCH__
#endif

#if defined(RTEMS_SMP) || \
   (CPU_INLINE_ENABLE_DISPATCH == FALSE) || \
   (__RTEMS_DO_NOT_INLINE_THREAD_ENABLE_DISPATCH__ == 1)
  #define __THREAD_DO_NOT_INLINE_ENABLE_DISPATCH__
#endif

/**
 * @addtogroup ScoreThread
 *
 * @{
 */

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

  _ISR_Disable_without_giant( level );
#endif

  enabled = _Thread_Dispatch_disable_level == 0;

#if defined(RTEMS_SMP)
  _ISR_Enable_without_giant( level );
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

#if defined(RTEMS_SMP)
  /**
   * @brief Acquires the giant lock.
   *
   * The giant lock is a recursive SMP lock protecting nearly all operating
   * system services.
   *
   * This lock is implicitly acquired in
   * _Thread_Dispatch_increment_disable_level().
   *
   * Thread dispatching must be disabled before the Giant lock can be acquired
   * and must no be enabled while owning the Giant lock.  The thread dispatch
   * disable level is not altered by this function.
   *
   * @param[in] cpu_self The current processor.
   */
  void _Giant_Acquire( Per_CPU_Control *cpu_self );

  /**
   * @brief Releases the giant lock.
   *
   * This lock is implicitly released in
   * _Thread_Dispatch_decrement_disable_level().
   *
   * The thread dispatch disable level is not altered by this function.
   *
   * @param[in] cpu_self The current processor.
   */
  void _Giant_Release( Per_CPU_Control *cpu_self );

  /**
   * @brief Releases the giant lock completely if held by the executing processor.
   *
   * The thread dispatch disable level is not altered by this function.
   *
   * The only use case for this operation is in _SMP_Request_shutdown().
   *
   * @param[in] cpu_self The current processor.
   */
  void _Giant_Drop( Per_CPU_Control *cpu_self );

  /**
   *  @brief Increments the thread dispatch level.
   *
   * This rountine increments the thread dispatch level
   */
  uint32_t _Thread_Dispatch_increment_disable_level(void);

  /**
   *  @brief Decrements the thread dispatch level.
   *
   * This routine decrements the thread dispatch level.
   */
  uint32_t _Thread_Dispatch_decrement_disable_level(void);
#else /* RTEMS_SMP */
  /**
   * @brief Increase thread dispatch disable level.
   *
   * This rountine increments the thread dispatch level
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_increment_disable_level(void)
  {
    uint32_t disable_level = _Thread_Dispatch_disable_level;
#if defined( RTEMS_PROFILING )
    ISR_Level level;

    _ISR_Disable( level );
    _Profiling_Thread_dispatch_disable( _Per_CPU_Get(), disable_level );
#endif

    ++disable_level;
    _Thread_Dispatch_disable_level = disable_level;

#if defined( RTEMS_PROFILING )
    _ISR_Enable( level );
#endif

    return disable_level;
  }

  /**
   * @brief Decrease thread dispatch disable level.
   *
   * This routine decrements the thread dispatch level.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_decrement_disable_level(void)
  {
    uint32_t disable_level = _Thread_Dispatch_disable_level;
#if defined( RTEMS_PROFILING )
    ISR_Level level;

    _ISR_Disable( level );
#endif

    --disable_level;
    _Thread_Dispatch_disable_level = disable_level;

#if defined( RTEMS_PROFILING )
    _Profiling_Thread_dispatch_enable( _Per_CPU_Get(), disable_level );
    _ISR_Enable( level );
#endif

    return disable_level;
  }

  RTEMS_INLINE_ROUTINE void _Giant_Acquire( Per_CPU_Control *cpu_self )
  {
    (void) cpu_self;
  }

  RTEMS_INLINE_ROUTINE void _Giant_Release( Per_CPU_Control *cpu_self )
  {
    (void) cpu_self;
  }
#endif /* RTEMS_SMP */

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
 * @brief Performs a thread dispatch on the current processor.
 *
 * On entry the thread dispatch disable level must be equal to one and
 * interrupts must be disabled.
 *
 * This function assumes that a thread dispatch is necessary.
 *
 * @param[in] cpu_self The current processor.
 * @param[in] level The previous interrupt level.
 *
 * @see _Thread_Dispatch().
 */
void _Thread_Do_dispatch( Per_CPU_Control *cpu_self, ISR_Level level );

/**
 * @brief Disables thread dispatching inside a critical section (interrupts
 * disabled).
 *
 * This function does not acquire the Giant lock.
 *
 * @param[in] lock_context The lock context of the corresponding
 * _ISR_lock_ISR_disable() that started the critical section.
 *
 * @return The current processor.
 */
RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Dispatch_disable_critical(
  const ISR_lock_Context *lock_context
)
{
  Per_CPU_Control *cpu_self;
  uint32_t         disable_level;

  cpu_self = _Per_CPU_Get();
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
 * @brief Disables thread dispatching.
 *
 * This function does not acquire the Giant lock.
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
 * May perfrom a thread dispatch if necessary as a side-effect.
 *
 * This function does not release the Giant lock.
 *
 * @param[in] cpu_self The current processor.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_enable( Per_CPU_Control *cpu_self )
{
  uint32_t disable_level = cpu_self->thread_dispatch_disable_level;

  if ( disable_level == 1 ) {
    ISR_Level level;

    _ISR_Disable_without_giant( level );

    if ( cpu_self->dispatch_necessary ) {
      _Thread_Do_dispatch( cpu_self, level );
    } else {
      cpu_self->thread_dispatch_disable_level = 0;
      _Profiling_Thread_dispatch_enable( cpu_self, 0 );
    }

    _ISR_Enable_without_giant( level );
  } else {
    cpu_self->thread_dispatch_disable_level = disable_level - 1;
  }
}

/**
 * @brief Disables thread dispatching and acquires the Giant lock.
 */
#if defined ( __THREAD_DO_NOT_INLINE_DISABLE_DISPATCH__ )
void _Thread_Disable_dispatch( void );
#else
RTEMS_INLINE_ROUTINE void _Thread_Disable_dispatch( void )
{
  _Thread_Dispatch_increment_disable_level();
  RTEMS_COMPILER_MEMORY_BARRIER();
}
#endif

RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch_body( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  _Giant_Release( cpu_self );
  _Thread_Dispatch_enable( cpu_self );
}

/**
 * @brief Enables thread dispatching and releases the Giant lock.
 *
 * May perfrom a thread dispatch if necessary as a side-effect.
 */
#if defined ( __THREAD_DO_NOT_INLINE_ENABLE_DISPATCH__ )
  void _Thread_Enable_dispatch( void );
#else
  /* inlining of enable dispatching must be true */
  RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch( void )
  {
    RTEMS_COMPILER_MEMORY_BARRIER();
    _Thread_Enable_dispatch_body();
  }
#endif

/**
 * @brief Enables thread dispatching and releases the Giant lock.
 *
 * @warning A thread dispatch is not performed as a side-effect.  Use this
 * function with
 */
RTEMS_INLINE_ROUTINE void _Thread_Unnest_dispatch( void )
{
  RTEMS_COMPILER_MEMORY_BARRIER();
  _Thread_Dispatch_decrement_disable_level();
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_THREADDISPATCH_H */
