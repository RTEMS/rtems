/**
 * @file
 *
 * @ingroup RTEMSScoreSMPLock
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSMPLock related to ticket locks.
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPLOCKTICKET_H
#define _RTEMS_SCORE_SMPLOCKTICKET_H

#include <rtems/score/cpuopts.h>

#if defined(RTEMS_SMP)

#include <rtems/score/atomic.h>
#include <rtems/score/smplockstats.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreSMPLock
 *
 * @{
 */

/**
 * @brief SMP ticket lock control.
 */
typedef struct {
  Atomic_Uint next_ticket;
  Atomic_Uint now_serving;
} SMP_ticket_lock_Control;

/**
 * @brief SMP ticket lock control initializer for static initialization.
 */
#define SMP_TICKET_LOCK_INITIALIZER \
  { \
    ATOMIC_INITIALIZER_UINT( 0U ), \
    ATOMIC_INITIALIZER_UINT( 0U ) \
  }

/**
 * @brief Initializes the SMP ticket lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in, out] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Initialize(
  SMP_ticket_lock_Control *lock
)
{
  _Atomic_Init_uint( &lock->next_ticket, 0U );
  _Atomic_Init_uint( &lock->now_serving, 0U );
}

/**
 * @brief Destroys the SMP ticket lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Destroy( SMP_ticket_lock_Control *lock )
{
  (void) lock;
}

/**
 * @brief Acquires the SMP ticket lock.
 *
 * @param[in, out] lock The lock to acquire.
 * @param stats The SMP lock statistics.
 * @param[out] stats_context The context for the statistics.
 */
static inline void _SMP_ticket_lock_Do_acquire(
  SMP_ticket_lock_Control *lock
#if defined(RTEMS_PROFILING)
  ,
  SMP_lock_Stats          *stats,
  SMP_lock_Stats_context  *stats_context
#endif
)
{
  unsigned int                   my_ticket;
  unsigned int                   now_serving;
#if defined(RTEMS_PROFILING)
  unsigned int                   initial_queue_length;
  SMP_lock_Stats_acquire_context acquire_context;

  _SMP_lock_Stats_acquire_begin( &acquire_context );
#endif

  my_ticket =
    _Atomic_Fetch_add_uint( &lock->next_ticket, 1U, ATOMIC_ORDER_RELAXED );

#if defined(RTEMS_PROFILING)
  now_serving =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_ACQUIRE );
  initial_queue_length = my_ticket - now_serving;

  if ( initial_queue_length > 0 ) {
#endif

    do {
      now_serving =
        _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_ACQUIRE );
    } while ( now_serving != my_ticket );

#if defined(RTEMS_PROFILING)
  }

  _SMP_lock_Stats_acquire_end(
    &acquire_context,
    stats,
    stats_context,
    initial_queue_length
  );
#endif
}

/**
 * @brief Acquires an SMP ticket lock.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite once it obtained
 * the SMP ticket lock.
 *
 * @param[in] lock The SMP ticket lock control.
 * @param[in] stats The SMP lock statistics.
 * @param[out] stats_context The SMP lock statistics context.
 */
#if defined(RTEMS_PROFILING)
  #define _SMP_ticket_lock_Acquire( lock, stats, stats_context ) \
    _SMP_ticket_lock_Do_acquire( lock, stats, stats_context )
#else
  #define _SMP_ticket_lock_Acquire( lock, stats, stats_context ) \
    _SMP_ticket_lock_Do_acquire( lock )
#endif

/**
 * @brief Releases the SMP ticket lock.
 *
 * @param[in, out] lock The SMP ticket lock to release.
 * @param[out] stats_context The SMP lock statistics context.
 */
static inline void _SMP_ticket_lock_Do_release(
  SMP_ticket_lock_Control *lock
#if defined(RTEMS_PROFILING)
  ,
  const SMP_lock_Stats_context *stats_context
#endif
)
{
  unsigned int current_ticket =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  unsigned int next_ticket = current_ticket + 1U;

#if defined(RTEMS_PROFILING)
  _SMP_lock_Stats_release_update( stats_context );
#endif

  _Atomic_Store_uint( &lock->now_serving, next_ticket, ATOMIC_ORDER_RELEASE );
}

/**
 * @brief Releases an SMP ticket lock.
 *
 * @param[in] lock The SMP ticket lock control.
 * @param[in] stats_context The SMP lock statistics context.
 */
#if defined(RTEMS_PROFILING)
  #define _SMP_ticket_lock_Release( lock, stats_context ) \
    _SMP_ticket_lock_Do_release( lock, stats_context )
#else
  #define _SMP_ticket_lock_Release( lock, stats_context ) \
    _SMP_ticket_lock_Do_release( lock )
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_SMPLOCKTICKET_H */
