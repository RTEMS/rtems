/**
 * @brief Constants and Structures Related with Thread Dispatch
 */

/*
 * COPYRIGHT (c) 1989-2009.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADDISPATCH_H
#define _RTEMS_SCORE_THREADDISPATCH_H

#include <rtems/score/cpu.h>
#include <rtems/score/smplock.h>

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
 *  The following declares the dispatch critical section nesting
 *  counter which is used to prevent context switches at inopportune
 *  moments.
 */
SCORE_EXTERN volatile uint32_t   _Thread_Dispatch_disable_level;

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
  return _Thread_Dispatch_disable_level == 0;
}

#if defined(RTEMS_SMP)
  typedef struct {
    SMP_lock_Control lock;
    uint32_t owner_cpu;
    uint32_t nest_level;
  } Thread_Dispatch_disable_level_lock_control;

  /**
   * The following declares the smp spinlock to be used to control
   * the dispatch critical section accesses across cpus.
   */
  SCORE_EXTERN Thread_Dispatch_disable_level_lock_control
    _Thread_Dispatch_disable_level_lock;

  /**
   *  @brief Initializes the thread dispatching subsystem.
   *
   *  This routine initializes the thread dispatching subsystem.
   */
  void _Thread_Dispatch_initialization(void);

  /**
   *  @brief Returns value of the the thread dispatch level.
   *
   * This routine returns value of the the thread dispatch level.
   */
  uint32_t _Thread_Dispatch_get_disable_level(void);

  /**
   *  @brief Sets thread dispatch level to the value passed in.
   *
   * This routine sets thread dispatch level to the
   * value passed in.
   */
  uint32_t _Thread_Dispatch_set_disable_level(uint32_t value);

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
   * @brief Get thread dispatch disable level.
   *
   * This routine returns value of the the thread dispatch level.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_get_disable_level(void)
  {
    return _Thread_Dispatch_disable_level;
  }

  /**
   * @brief Set thread dispatch disable level.
   *
   * This routine sets thread dispatch level to the
   * value passed in.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_set_disable_level(uint32_t value)
  {
    _Thread_Dispatch_disable_level = value;
    return value;
  }

  /**
   * @brief Increase thread dispatch disable level.
   *
   * This rountine increments the thread dispatch level
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_increment_disable_level(void)
  {
    uint32_t level = _Thread_Dispatch_disable_level;

    ++level;
    _Thread_Dispatch_disable_level = level;

    return level;
  }

  /**
   * @brief Decrease thread dispatch disable level.
   *
   * This routine decrements the thread dispatch level.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_decrement_disable_level(void)
  {
    uint32_t level = _Thread_Dispatch_disable_level;

    --level;
    _Thread_Dispatch_disable_level = level;

    return level;
  }

  /**
   * @brief Thread dispatch initialization.
   *
   * This routine initializes the thread dispatching subsystem.
   */
  RTEMS_INLINE_ROUTINE void _Thread_Dispatch_initialization( void )
  {
    _Thread_Dispatch_set_disable_level( 1 );
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
