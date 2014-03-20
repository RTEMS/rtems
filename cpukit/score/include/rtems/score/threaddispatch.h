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
#include <rtems/score/smplock.h>
#include <rtems/score/profiling.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTEMS_SMP) || \
    defined(RTEMS_HEAVY_STACK_DEBUG) || \
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
 * @briefs Gets thread dispatch disable level.
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
   * Thread dispatching must be disabled before this lock can be acquired.
   */
  void _Giant_Acquire( void );

  /**
   * @brief Releases the giant lock.
   *
   * This lock is implicitly released in
   * _Thread_Dispatch_decrement_disable_level().
   */
  void _Giant_Release( void );

  /**
   * @brief Releases the giant lock completely if held by the executing processor.
   *
   * The thread dispatch disable level is not altered by this function.
   *
   * The only use case for this operation is in _SMP_Request_shutdown().
   *
   * @param[in] self_cpu The current processor.
   */
  void _Giant_Drop( Per_CPU_Control *self_cpu );

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
#endif /* RTEMS_SMP */

/**
 *  @brief Dispatch thread.
 *
 *  This routine is responsible for transferring control of the
 *  processor from the executing thread to the heir thread. Once the
 *  heir is running an attempt is made to dispatch any ASRs.
 *  As part of this process, it is responsible for the following actions:
 *     + saving the context of the executing thread
 *     + restoring the context of the heir thread
 *     + dispatching any signals for the resulting executing thread

 *  ALTERNATE ENTRY POINTS:
 *    void _Thread_Enable_dispatch();
 *
 *  - INTERRUPT LATENCY:
 *    + dispatch thread
 *    + no dispatch thread
 */
void _Thread_Dispatch( void );

/**
 * This routine prevents dispatching.
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

/**
 * This routine allows dispatching to occur again.  If this is
 * the outer most dispatching critical section, then a dispatching
 * operation will be performed and, if necessary, control of the
 * processor will be transferred to the heir thread.
 */

#if defined ( __THREAD_DO_NOT_INLINE_ENABLE_DISPATCH__ )
  void _Thread_Enable_dispatch( void );
#else
  /* inlining of enable dispatching must be true */
  RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch( void )
  {
    RTEMS_COMPILER_MEMORY_BARRIER();
    if ( _Thread_Dispatch_decrement_disable_level() == 0 )
      _Thread_Dispatch();
  }
#endif

/**
 * This routine allows dispatching to occur again.  However,
 * no dispatching operation is performed even if this is the outer
 * most dispatching critical section.
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
