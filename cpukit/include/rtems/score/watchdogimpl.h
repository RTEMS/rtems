/**
 * @file
 *
 * @brief Inlined Routines in the Watchdog Handler
 *
 * This file contains the static inline implementation of all inlined
 * routines in the Watchdog Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WATCHDOGIMPL_H
#define _RTEMS_SCORE_WATCHDOGIMPL_H

#include <rtems/score/watchdog.h>
#include <rtems/score/watchdogticks.h>
#include <rtems/score/assert.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/percpu.h>
#include <rtems/score/rbtreeimpl.h>

#include <sys/types.h>
#include <sys/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreWatchdog
 *  @{
 */

/**
 * @brief Watchdog states.
 */
typedef enum {
  /**
   * @brief The watchdog is scheduled and a black node in the red-black tree.
   */
  WATCHDOG_SCHEDULED_BLACK,

  /**
   * @brief The watchdog is scheduled and a red node in the red-black tree.
   */
  WATCHDOG_SCHEDULED_RED,

  /**
   * @brief The watchdog is inactive.
   */
  WATCHDOG_INACTIVE,

  /**
   * @brief The watchdog is on a chain of pending watchdogs.
   *
   * This state is used by the timer server for example.
   */
  WATCHDOG_PENDING
} Watchdog_State;

/**
 * @brief Watchdog initializer for static initialization.
 *
 * The processor of this watchdog is set to processor with index zero.
 *
 * @see _Watchdog_Preinitialize().
 */
#if defined(RTEMS_SMP)
  #define WATCHDOG_INITIALIZER( routine ) \
    { \
      { { { NULL, NULL, NULL, WATCHDOG_INACTIVE } } }, \
      &_Per_CPU_Information[ 0 ].per_cpu, \
      ( routine ), \
      0 \
    }
#else
  #define WATCHDOG_INITIALIZER( routine ) \
    { \
      { { { NULL, NULL, NULL, WATCHDOG_INACTIVE } } }, \
      ( routine ), \
      0 \
    }
#endif

RTEMS_INLINE_ROUTINE void _Watchdog_Header_initialize(
  Watchdog_Header *header
)
{
  _RBTree_Initialize_empty( &header->Watchdogs );
  header->first = NULL;
}

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Header_first(
  const Watchdog_Header *header
)
{
  return (Watchdog_Control *) header->first;
}

RTEMS_INLINE_ROUTINE void _Watchdog_Header_destroy(
  Watchdog_Header *header
)
{
  /* Do nothing */
  (void) header;
}

/**
 *  @brief Performs a watchdog tick.
 *
 *  @param cpu The processor for this watchdog tick.
 */
void _Watchdog_Tick( struct Per_CPU_Control *cpu );

RTEMS_INLINE_ROUTINE Watchdog_State _Watchdog_Get_state(
  const Watchdog_Control *the_watchdog
)
{
  return RB_COLOR( &the_watchdog->Node.RBTree, Node );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Set_state(
  Watchdog_Control *the_watchdog,
  Watchdog_State    state
)
{
  RB_COLOR( &the_watchdog->Node.RBTree, Node ) = state;
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Watchdog_Get_CPU(
  const Watchdog_Control *the_watchdog
)
{
#if defined(RTEMS_SMP)
  return the_watchdog->cpu;
#else
  return _Per_CPU_Get_by_index( 0 );
#endif
}

RTEMS_INLINE_ROUTINE void _Watchdog_Set_CPU(
  Watchdog_Control *the_watchdog,
  Per_CPU_Control  *cpu
)
{
#if defined(RTEMS_SMP)
  the_watchdog->cpu = cpu;
#else
  (void) cpu;
#endif
}

/**
 * @brief Pre-initializes a watchdog.
 *
 * This routine must be called before a watchdog is used in any way.  The
 * exception are statically initialized watchdogs via WATCHDOG_INITIALIZER().
 *
 * @param[in] the_watchdog The uninitialized watchdog.
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Preinitialize(
  Watchdog_Control *the_watchdog,
  Per_CPU_Control  *cpu
)
{
  _Watchdog_Set_CPU( the_watchdog, cpu );
  _Watchdog_Set_state( the_watchdog, WATCHDOG_INACTIVE );

#if defined(RTEMS_DEBUG)
  the_watchdog->routine = NULL;
  the_watchdog->expire = 0;
#endif
}

/**
 * @brief Initializes a watchdog with a new service routine.
 *
 * The watchdog must be inactive.
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Initialize(
  Watchdog_Control               *the_watchdog,
  Watchdog_Service_routine_entry  routine
)
{
  _Assert( _Watchdog_Get_state( the_watchdog ) == WATCHDOG_INACTIVE );
  the_watchdog->routine = routine;
}

void _Watchdog_Do_tickle(
  Watchdog_Header  *header,
  Watchdog_Control *first,
  uint64_t          now,
#if defined(RTEMS_SMP)
  ISR_lock_Control *lock,
#endif
  ISR_lock_Context *lock_context
);

#if defined(RTEMS_SMP)
  #define _Watchdog_Tickle( header, first, now, lock, lock_context ) \
    _Watchdog_Do_tickle( header, first, now, lock, lock_context )
#else
  #define _Watchdog_Tickle( header, first, now, lock, lock_context ) \
    _Watchdog_Do_tickle( header, first, now, lock_context )
#endif

/**
 * @brief Inserts a watchdog into the set of scheduled watchdogs according to
 * the specified expiration time.
 *
 * The watchdog must be inactive.
 */
void _Watchdog_Insert(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog,
  uint64_t          expire
);

/**
 * @brief In case the watchdog is scheduled, then it is removed from the set of
 * scheduled watchdogs.
 *
 * The watchdog must be initialized before this call.
 */
void _Watchdog_Remove(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
);

/**
 * @brief In case the watchdog is scheduled, then it is removed from the set of
 * scheduled watchdogs.
 *
 * The watchdog must be initialized before this call.
 *
 * @retval 0 The now time is greater than or equal to the expiration time of
 * the watchdog.
 * @retval other The difference of the now and expiration time.
 */
RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Cancel(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog,
  uint64_t          now
)
{
  uint64_t expire;
  uint64_t remaining;

  expire = the_watchdog->expire;

  if ( now < expire ) {
    remaining = expire - now;
  } else {
    remaining = 0;
  }

  _Watchdog_Remove( header, the_watchdog );

  return remaining;
}

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_scheduled(
  const Watchdog_Control *the_watchdog
)
{
  return _Watchdog_Get_state( the_watchdog ) < WATCHDOG_INACTIVE;
}

RTEMS_INLINE_ROUTINE void _Watchdog_Next_first(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
)
{
  RBTree_Node *node = _RBTree_Right( &the_watchdog->Node.RBTree );

  if ( node != NULL ) {
    RBTree_Node *left;

    while ( ( left = _RBTree_Left( node ) ) != NULL ) {
      node = left;
    }

    header->first = node;
  } else {
    header->first = _RBTree_Parent( &the_watchdog->Node.RBTree );
  }
}

/**
 * @brief The maximum watchdog ticks value for the far future.
 */
#define WATCHDOG_MAXIMUM_TICKS UINT64_MAX

#define WATCHDOG_NANOSECONDS_PER_SECOND 1000000000

/**
 * @brief The bits necessary to store 1000000000
 * (= WATCHDOG_NANOSECONDS_PER_SECOND) nanoseconds.
 *
 * The expiration time is an unsigned 64-bit integer.  To store nanoseconds
 * timeouts we use 30 bits (2**30 == 1073741824) for the nanoseconds and 34
 * bits for the seconds since UNIX Epoch.  This leads to a year 2514 problem.
 */
#define WATCHDOG_BITS_FOR_1E9_NANOSECONDS 30

/**
 * @brief The maximum number of seconds representable in the nanoseconds
 * watchdog format.
 *
 * We have 2**34 bits for the seconds part.
 */
#define WATCHDOG_MAX_SECONDS 0x3ffffffff

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_valid_timespec(
  const struct timespec *ts
)
{
  return ts != NULL
    && (unsigned long) ts->tv_nsec < WATCHDOG_NANOSECONDS_PER_SECOND;
}

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_valid_interval_timespec(
  const struct timespec *ts
)
{
  return _Watchdog_Is_valid_timespec( ts ) && ts->tv_sec >= 0;
}

RTEMS_INLINE_ROUTINE const struct timespec * _Watchdog_Future_timespec(
  struct timespec       *now,
  const struct timespec *delta
)
{
  uint64_t sec;

  if ( !_Watchdog_Is_valid_interval_timespec( delta ) ) {
    return NULL;
  }

  sec = (uint64_t) now->tv_sec;
  sec += (uint64_t) delta->tv_sec;
  now->tv_nsec += delta->tv_nsec;

  /* We have 2 * (2**63 - 1) + 1 == UINT64_MAX */
  if ( now->tv_nsec >= WATCHDOG_NANOSECONDS_PER_SECOND ) {
    now->tv_nsec -= WATCHDOG_NANOSECONDS_PER_SECOND;
    ++sec;
  }

  if ( sec <= INT64_MAX ) {
    now->tv_sec = sec;
  } else {
    now->tv_sec = INT64_MAX;
  }

  return now;
}

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_far_future_timespec(
  const struct timespec *ts
)
{
  return ts->tv_sec > WATCHDOG_MAX_SECONDS;
}

RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Ticks_from_seconds(
  uint32_t seconds
)
{
  uint64_t ticks = seconds;

  ticks <<= WATCHDOG_BITS_FOR_1E9_NANOSECONDS;

  return ticks;
}

RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Ticks_from_timespec(
  const struct timespec *ts
)
{
  uint64_t ticks;

  _Assert( _Watchdog_Is_valid_timespec( ts ) );
  _Assert( ts->tv_sec >= 0 );
  _Assert( !_Watchdog_Is_far_future_timespec( ts ) );

  ticks = (uint64_t) ts->tv_sec;
  ticks <<= WATCHDOG_BITS_FOR_1E9_NANOSECONDS;
  ticks |= (uint32_t) ts->tv_nsec;

  return ticks;
}

RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Ticks_from_sbintime(
  sbintime_t sbt
)
{
  uint64_t ticks = ( sbt >> 32 ) << WATCHDOG_BITS_FOR_1E9_NANOSECONDS;

  ticks |= ( (uint64_t) 1000000000 * (uint32_t) sbt ) >> 32;

  return ticks;
}

RTEMS_INLINE_ROUTINE void _Watchdog_Per_CPU_acquire_critical(
  Per_CPU_Control  *cpu,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &cpu->Watchdog.Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Per_CPU_release_critical(
  Per_CPU_Control  *cpu,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &cpu->Watchdog.Lock, lock_context );
}

RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Per_CPU_insert_ticks(
  Watchdog_Control  *the_watchdog,
  Per_CPU_Control   *cpu,
  Watchdog_Interval  ticks
)
{
  ISR_lock_Context  lock_context;
  Watchdog_Header  *header;
  uint64_t          expire;

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];

  _Watchdog_Set_CPU( the_watchdog, cpu );

  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context );
  expire = ticks + cpu->Watchdog.ticks;
  _Watchdog_Insert(header, the_watchdog, expire);
  _Watchdog_Per_CPU_release_critical( cpu, &lock_context );
  return expire;
}

RTEMS_INLINE_ROUTINE uint64_t _Watchdog_Per_CPU_insert(
  Watchdog_Control *the_watchdog,
  Per_CPU_Control  *cpu,
  Watchdog_Header  *header,
  uint64_t          expire
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Set_CPU( the_watchdog, cpu );

  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context );
  _Watchdog_Insert( header, the_watchdog, expire );
  _Watchdog_Per_CPU_release_critical( cpu, &lock_context );
  return expire;
}

RTEMS_INLINE_ROUTINE void _Watchdog_Per_CPU_remove(
  Watchdog_Control *the_watchdog,
  Per_CPU_Control  *cpu,
  Watchdog_Header  *header
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context );
  _Watchdog_Remove(
    header,
    the_watchdog
  );
  _Watchdog_Per_CPU_release_critical( cpu, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Per_CPU_remove_ticks(
  Watchdog_Control *the_watchdog
)
{
  Per_CPU_Control *cpu;

  cpu = _Watchdog_Get_CPU( the_watchdog );
  _Watchdog_Per_CPU_remove(
    the_watchdog,
    cpu,
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ]
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
