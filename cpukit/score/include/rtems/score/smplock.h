/**
 * @file
 *
 * @ingroup ScoreSMPLock
 *
 * @brief SMP Lock API
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2013-2015 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPLOCK_H
#define _RTEMS_SCORE_SMPLOCK_H

#include <rtems/score/cpuopts.h>

#if defined( RTEMS_SMP )

#include <rtems/score/atomic.h>
#include <rtems/score/isrlevel.h>

#if defined( RTEMS_PROFILING )
#include <rtems/score/chainimpl.h>
#include <string.h>
#endif

#if defined( RTEMS_PROFILING )
#define RTEMS_SMP_LOCK_DO_NOT_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPLock SMP Locks
 *
 * @ingroup Score
 *
 * @brief The SMP lock provides mutual exclusion for SMP systems at the lowest
 * level.
 *
 * The SMP lock is implemented as a ticket lock.  This provides fairness in
 * case of concurrent lock attempts.
 *
 * This SMP lock API uses a local context for acquire and release pairs.  Such
 * a context may be used to implement for example the Mellor-Crummey and Scott
 * (MCS) locks in the future.
 *
 * @{
 */

#if defined( RTEMS_PROFILING )

/**
 * @brief Count of lock contention counters for lock statistics.
 */
#define SMP_LOCK_STATS_CONTENTION_COUNTS 4

/**
 * @brief SMP lock statistics.
 *
 * The lock acquire attempt instant is the point in time right after the
 * interrupt disable action in the lock acquire sequence.
 *
 * The lock acquire instant is the point in time right after the lock
 * acquisition.  This is the begin of the critical section code execution.
 *
 * The lock release instant is the point in time right before the interrupt
 * enable action in the lock release sequence.
 *
 * The lock section time is the time elapsed between the lock acquire instant
 * and the lock release instant.
 *
 * The lock acquire time is the time elapsed between the lock acquire attempt
 * instant and the lock acquire instant.
 */
typedef struct {
  /**
   * @brief Node for SMP lock statistics chain.
   */
  Chain_Node Node;

  /**
   * @brief The maximum lock acquire time in CPU counter ticks.
   */
  CPU_Counter_ticks max_acquire_time;

  /**
   * @brief The maximum lock section time in CPU counter ticks.
   */
  CPU_Counter_ticks max_section_time;

  /**
   * @brief The count of lock uses.
   *
   * This value may overflow.
   */
  uint64_t usage_count;

  /**
   * @brief Total lock acquire time in nanoseconds.
   *
   * The average lock acquire time is the total acquire time divided by the
   * lock usage count.  The ration of the total section and total acquire times
   * gives a measure for the lock contention.
   *
   * This value may overflow.
   */
  uint64_t total_acquire_time;

  /**
   * @brief The counts of lock acquire operations by contention.
   *
   * The contention count for index N corresponds to a lock acquire attempt
   * with an initial queue length of N.  The last index corresponds to all
   * lock acquire attempts with an initial queue length greater than or equal
   * to SMP_LOCK_STATS_CONTENTION_COUNTS minus one.
   *
   * The values may overflow.
   */
  uint64_t contention_counts[SMP_LOCK_STATS_CONTENTION_COUNTS];

  /**
   * @brief Total lock section time in CPU counter ticks.
   *
   * The average lock section time is the total section time divided by the
   * lock usage count.
   *
   * This value may overflow.
   */
  uint64_t total_section_time;

  /**
   * @brief The lock name.
   */
  const char *name;
} SMP_lock_Stats;

/**
 * @brief Local context for SMP lock statistics.
 */
typedef struct {
  /**
   * @brief The last lock acquire instant in CPU counter ticks.
   *
   * This value is used to measure the lock section time.
   */
  CPU_Counter_ticks acquire_instant;

  /**
   * @brief The lock stats used for the last lock acquire.
   */
  SMP_lock_Stats *stats;
} SMP_lock_Stats_context;

/**
 * @brief SMP lock statistics initializer for static initialization.
 */
#define SMP_LOCK_STATS_INITIALIZER( name ) \
  { { NULL, NULL }, 0, 0, 0, 0, { 0, 0, 0, 0 }, 0, name }

/**
 * @brief Initializes an SMP lock statistics block.
 *
 * @param[in, out] stats The SMP lock statistics block.
 * @param[in] name The name for the SMP lock statistics.  This name must be
 * persistent throughout the life time of this statistics block.
 */
static inline void _SMP_lock_Stats_initialize(
  SMP_lock_Stats *stats,
  const char *name
)
{
  SMP_lock_Stats init = SMP_LOCK_STATS_INITIALIZER( name );

  *stats = init;
}

/**
 * @brief Destroys an SMP lock statistics block.
 *
 * @param[in] stats The SMP lock statistics block.
 */
static inline void _SMP_lock_Stats_destroy( SMP_lock_Stats *stats );

/**
 * @brief Updates an SMP lock statistics block during a lock release.
 *
 * @param[in] stats_context The SMP lock statistics context.
 */
static inline void _SMP_lock_Stats_release_update(
  const SMP_lock_Stats_context *stats_context
);

#else /* RTEMS_PROFILING */

#define _SMP_lock_Stats_initialize( stats, name ) do { } while ( 0 )

#define _SMP_lock_Stats_destroy( stats ) do { } while ( 0 )

#endif /* RTEMS_PROFILING */

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
 * @brief Initializes an SMP ticket lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in] lock The SMP ticket lock control.
 * @param[in] name The name for the SMP ticket lock.  This name must be
 * persistent throughout the life time of this lock.
 */
static inline void _SMP_ticket_lock_Initialize(
  SMP_ticket_lock_Control *lock
)
{
  _Atomic_Init_uint( &lock->next_ticket, 0U );
  _Atomic_Init_uint( &lock->now_serving, 0U );
}

/**
 * @brief Destroys an SMP ticket lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Destroy( SMP_ticket_lock_Control *lock )
{
  (void) lock;
}

static inline void _SMP_ticket_lock_Do_acquire(
  SMP_ticket_lock_Control *lock
#if defined( RTEMS_PROFILING )
  ,
  SMP_lock_Stats *stats,
  SMP_lock_Stats_context *stats_context
#endif
)
{
  unsigned int my_ticket;
  unsigned int now_serving;

#if defined( RTEMS_PROFILING )
  CPU_Counter_ticks first;
  CPU_Counter_ticks second;
  CPU_Counter_ticks delta;
  unsigned int initial_queue_length;

  first = _CPU_Counter_read();
#endif

  my_ticket =
    _Atomic_Fetch_add_uint( &lock->next_ticket, 1U, ATOMIC_ORDER_ACQ_REL );

#if defined( RTEMS_PROFILING )
  now_serving =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_ACQUIRE );
  initial_queue_length = my_ticket - now_serving;

  if ( initial_queue_length > 0 ) {
#endif

    do {
      now_serving =
        _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_ACQUIRE );
    } while ( now_serving != my_ticket );

#if defined( RTEMS_PROFILING )
  }

  second = _CPU_Counter_read();
  stats_context->acquire_instant = second;
  delta = _CPU_Counter_difference( second, first );

  ++stats->usage_count;

  stats->total_acquire_time += delta;

  if ( stats->max_acquire_time < delta ) {
    stats->max_acquire_time = delta;
  }

  if ( initial_queue_length >= SMP_LOCK_STATS_CONTENTION_COUNTS ) {
    initial_queue_length = SMP_LOCK_STATS_CONTENTION_COUNTS - 1;
  }
  ++stats->contention_counts[initial_queue_length];

  stats_context->stats = stats;
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
#if defined( RTEMS_PROFILING )
  #define _SMP_ticket_lock_Acquire( lock, stats, stats_context ) \
    _SMP_ticket_lock_Do_acquire( lock, stats, stats_context )
#else
  #define _SMP_ticket_lock_Acquire( lock, stats, stats_context ) \
    _SMP_ticket_lock_Do_acquire( lock )
#endif

static inline void _SMP_ticket_lock_Do_release(
  SMP_ticket_lock_Control *lock
#if defined( RTEMS_PROFILING )
  ,
  const SMP_lock_Stats_context *stats_context
#endif
)
{
  unsigned int current_ticket =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  unsigned int next_ticket = current_ticket + 1U;

#if defined( RTEMS_PROFILING )
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
#if defined( RTEMS_PROFILING )
  #define _SMP_ticket_lock_Release( lock, stats_context ) \
    _SMP_ticket_lock_Do_release( lock, stats_context )
#else
  #define _SMP_ticket_lock_Release( lock, stats_context ) \
    _SMP_ticket_lock_Do_release( lock )
#endif

/**
 * @brief SMP lock control.
 */
typedef struct {
  SMP_ticket_lock_Control Ticket_lock;
#if defined( RTEMS_PROFILING )
  SMP_lock_Stats Stats;
#endif
} SMP_lock_Control;

/**
 * @brief Local SMP lock context for acquire and release pairs.
 */
typedef struct {
  ISR_Level isr_level;
#if defined( RTEMS_PROFILING )
  SMP_lock_Stats_context Stats_context;
#endif
} SMP_lock_Context;

/**
 * @brief SMP lock control initializer for static initialization.
 */
#if defined( RTEMS_PROFILING )
  #define SMP_LOCK_INITIALIZER( name ) \
    { SMP_TICKET_LOCK_INITIALIZER, SMP_LOCK_STATS_INITIALIZER( name ) }
#else
  #define SMP_LOCK_INITIALIZER( name ) { SMP_TICKET_LOCK_INITIALIZER }
#endif

/**
 * @brief Initializes an SMP lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in] lock The SMP lock control.
 * @param[in] name The name for the SMP lock statistics.  This name must be
 * persistent throughout the life time of this statistics block.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_Initialize(
  SMP_lock_Control *lock,
  const char *name
);

static inline void _SMP_lock_Initialize_body(
#else
static inline void _SMP_lock_Initialize(
#endif
  SMP_lock_Control *lock,
  const char *name
)
{
  _SMP_ticket_lock_Initialize( &lock->Ticket_lock );
#if defined( RTEMS_PROFILING )
  _SMP_lock_Stats_initialize( &lock->Stats, name );
#else
  (void) name;
#endif
}

/**
 * @brief Destroys an SMP lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in] lock The SMP lock control.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_Destroy( SMP_lock_Control *lock );

static inline void _SMP_lock_Destroy_body( SMP_lock_Control *lock )
#else
static inline void _SMP_lock_Destroy( SMP_lock_Control *lock )
#endif
{
  _SMP_ticket_lock_Destroy( &lock->Ticket_lock );
  _SMP_lock_Stats_destroy( &lock->Stats );
}

/**
 * @brief Acquires an SMP lock.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite once it obtained
 * the SMP lock.
 *
 * @param[in] lock The SMP lock control.
 * @param[in] context The local SMP lock context for an acquire and release
 * pair.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_Acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
);

static inline void _SMP_lock_Acquire_body(
#else
static inline void _SMP_lock_Acquire(
#endif
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  (void) context;
  _SMP_ticket_lock_Acquire(
    &lock->Ticket_lock,
    &lock->Stats,
    &context->Stats_context
  );
}

/**
 * @brief Releases an SMP lock.
 *
 * @param[in] lock The SMP lock control.
 * @param[in] context The local SMP lock context for an acquire and release
 * pair.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_Release(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
);

static inline void _SMP_lock_Release_body(
#else
static inline void _SMP_lock_Release(
#endif
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  (void) context;
  _SMP_ticket_lock_Release(
    &lock->Ticket_lock,
    &context->Stats_context
  );
}

/**
 * @brief Disables interrupts and acquires the SMP lock.
 *
 * @param[in] lock The SMP lock control.
 * @param[in] context The local SMP lock context for an acquire and release
 * pair.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_ISR_disable_and_acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
);

static inline void _SMP_lock_ISR_disable_and_acquire_body(
#else
static inline void _SMP_lock_ISR_disable_and_acquire(
#endif
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _ISR_Disable_without_giant( context->isr_level );
  _SMP_lock_Acquire( lock, context );
}

/**
 * @brief Releases the SMP lock and enables interrupts.
 *
 * @param[in] lock The SMP lock control.
 * @param[in] context The local SMP lock context for an acquire and release
 * pair.
 */
#if defined( RTEMS_SMP_LOCK_DO_NOT_INLINE )
void _SMP_lock_Release_and_ISR_enable(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
);

static inline void _SMP_lock_Release_and_ISR_enable_body(
#else
static inline void _SMP_lock_Release_and_ISR_enable(
#endif
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_Release( lock, context );
  _ISR_Enable_without_giant( context->isr_level );
}

#if defined( RTEMS_PROFILING )

typedef struct {
  SMP_lock_Control Lock;
  Chain_Control Stats_chain;
  Chain_Control Iterator_chain;
} SMP_lock_Stats_control;

typedef struct {
  Chain_Node Node;
  SMP_lock_Stats *current;
} SMP_lock_Stats_iteration_context;

extern SMP_lock_Stats_control _SMP_lock_Stats_control;

static inline void _SMP_lock_Stats_iteration_start(
  SMP_lock_Stats_iteration_context *iteration_context
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

  _Chain_Append_unprotected(
    &control->Iterator_chain,
    &iteration_context->Node
  );
  iteration_context->current =
    (SMP_lock_Stats *) _Chain_First( &control->Stats_chain );

  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
}

static inline bool _SMP_lock_Stats_iteration_next(
  SMP_lock_Stats_iteration_context *iteration_context,
  SMP_lock_Stats *snapshot,
  char *name,
  size_t name_size
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;
  SMP_lock_Stats *current;
  bool valid;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

  current = iteration_context->current;
  if ( !_Chain_Is_tail( &control->Stats_chain, &current->Node ) ) {
    size_t name_len = current->name != NULL ? strlen(current->name) : 0;

    valid = true;

    iteration_context->current = (SMP_lock_Stats *)
      _Chain_Next( &current->Node );

    *snapshot = *current;
    snapshot->name = name;

    if ( name_len >= name_size ) {
      name_len = name_size - 1;
    }

    name[name_len] = '\0';
    memcpy(name, current->name, name_len);
  } else {
    valid = false;
  }

  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );

  return valid;
}

static inline void _SMP_lock_Stats_iteration_stop(
  SMP_lock_Stats_iteration_context *iteration_context
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );
  _Chain_Extract_unprotected( &iteration_context->Node );
  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
}

static inline void _SMP_lock_Stats_destroy( SMP_lock_Stats *stats )
{
  if ( !_Chain_Is_node_off_chain( &stats->Node ) ) {
    SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
    SMP_lock_Context lock_context;
    SMP_lock_Stats_iteration_context *iteration_context;
    SMP_lock_Stats_iteration_context *iteration_context_tail;
    SMP_lock_Stats *next_stats;

    _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

    next_stats = (SMP_lock_Stats *) _Chain_Next( &stats->Node );
    _Chain_Extract_unprotected( &stats->Node );

    iteration_context = (SMP_lock_Stats_iteration_context *)
      _Chain_First( &control->Iterator_chain );
    iteration_context_tail = (SMP_lock_Stats_iteration_context *)
      _Chain_Tail( &control->Iterator_chain );

    while ( iteration_context != iteration_context_tail ) {
      if ( iteration_context->current == stats ) {
        iteration_context->current = next_stats;
      }

      iteration_context = (SMP_lock_Stats_iteration_context *)
        _Chain_Next( &iteration_context->Node );
    }

    _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
  }
}

static inline void _SMP_lock_Stats_release_update(
  const SMP_lock_Stats_context *stats_context
)
{
  SMP_lock_Stats *stats = stats_context->stats;
  CPU_Counter_ticks first = stats_context->acquire_instant;
  CPU_Counter_ticks second = _CPU_Counter_read();
  CPU_Counter_ticks delta = _CPU_Counter_difference( second, first );

  stats->total_section_time += delta;

  if ( stats->max_section_time < delta ) {
    stats->max_section_time = delta;

    if ( _Chain_Is_node_off_chain( &stats->Node ) ) {
      SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
      SMP_lock_Context lock_context;

      _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );
      _Chain_Append_unprotected( &control->Stats_chain, &stats->Node );
      _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
    }
  }
}

#endif /* RTEMS_PROFILING */

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined( RTEMS_SMP ) */

#endif /* _RTEMS_SCORE_SMPLOCK_H */
