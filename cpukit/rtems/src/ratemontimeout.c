/**
 *  @file
 *
 *  @brief Rate Monotonic Timeout
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  COPYRIGHT (c) 2016-2017 Kuan-Hsun Chen.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>

static void _Rate_monotonic_Renew_deadline(
  Rate_monotonic_Control *the_period,
  ISR_lock_Context       *lock_context
)
{
  uint64_t deadline;

  /* stay at 0xffffffff if postponed_jobs is going to overflow */
  if ( the_period->postponed_jobs != UINT32_MAX ) {
    ++the_period->postponed_jobs;
  }

  the_period->state = RATE_MONOTONIC_EXPIRED;

  deadline = _Watchdog_Per_CPU_insert_ticks(
    &the_period->Timer,
    _Per_CPU_Get(),
    the_period->next_length
  );
  the_period->latest_deadline = deadline;

  _Rate_monotonic_Release( the_period, lock_context );
}

void _Rate_monotonic_Timeout( Watchdog_Control *the_watchdog )
{
  Rate_monotonic_Control *the_period;
  Thread_Control         *owner;
  ISR_lock_Context        lock_context;
  Thread_Wait_flags       wait_flags;

  the_period = RTEMS_CONTAINER_OF( the_watchdog, Rate_monotonic_Control, Timer );
  owner = the_period->owner;

  _ISR_lock_ISR_disable( &lock_context );
  _Rate_monotonic_Acquire_critical( the_period, &lock_context );
  wait_flags = _Thread_Wait_flags_get( owner );

  if (
    ( wait_flags & THREAD_WAIT_CLASS_PERIOD ) != 0
      && owner->Wait.return_argument == the_period
  ) {
    bool unblock;
    bool success;

    owner->Wait.return_argument = NULL;

    success = _Thread_Wait_flags_try_change_release(
      owner,
      RATE_MONOTONIC_INTEND_TO_BLOCK,
      RATE_MONOTONIC_READY_AGAIN
    );
    if ( success ) {
      unblock = false;
    } else {
      _Assert( _Thread_Wait_flags_get( owner ) == RATE_MONOTONIC_BLOCKED );
      _Thread_Wait_flags_set( owner, RATE_MONOTONIC_READY_AGAIN );
      unblock = true;
    }

    _Rate_monotonic_Restart( the_period, owner, &lock_context );

    if ( unblock ) {
      _Thread_Unblock( owner );
    }
  } else {
    _Rate_monotonic_Renew_deadline( the_period, &lock_context );
  }
}
