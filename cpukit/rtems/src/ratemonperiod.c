/**
 *  @file
 *
 *  @brief Rate Monotonic Support
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/todimpl.h>

bool _Rate_monotonic_Get_status(
  const Rate_monotonic_Control *the_period,
  Timestamp_Control            *wall_since_last_period,
  Timestamp_Control            *cpu_since_last_period
)
{
  Timestamp_Control        uptime;
  Thread_Control          *owning_thread = the_period->owner;
  Timestamp_Control        used;

  /*
   *  Determine elapsed wall time since period initiated.
   */
  _TOD_Get_uptime( &uptime );
  _Timestamp_Subtract(
    &the_period->time_period_initiated, &uptime, wall_since_last_period
  );

  /*
   *  Determine cpu usage since period initiated.
   */
  _Thread_Get_CPU_time_used( owning_thread, &used );

  /*
   *  The cpu usage info was reset while executing.  Can't
   *  determine a status.
   */
  if ( _Timestamp_Less_than( &used, &the_period->cpu_usage_period_initiated ) )
    return false;

   /* used = current cpu usage - cpu usage at start of period */
  _Timestamp_Subtract(
    &the_period->cpu_usage_period_initiated,
    &used,
    cpu_since_last_period
  );

  return true;
}

static void _Rate_monotonic_Release_postponedjob(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  rtems_interval          next_length,
  ISR_lock_Context       *lock_context
)
{
  /* This function only handles the release of postponed jobs. */
  Per_CPU_Control *cpu_self;
  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( owner, lock_context );
  
  the_period->postponed_jobs -=1;
  _Scheduler_Release_job( owner, the_period->latest_deadline );

  _Thread_Dispatch_enable( cpu_self );
}

static void _Rate_monotonic_Release_job(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  rtems_interval          next_length,
  ISR_lock_Context       *lock_context
)
{
  Per_CPU_Control *cpu_self;
  uint64_t deadline;

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( owner, lock_context );

  _ISR_lock_ISR_disable( lock_context );
  deadline = _Watchdog_Per_CPU_insert_relative(
    &the_period->Timer,
    cpu_self,
    next_length
  );
  _ISR_lock_ISR_enable( lock_context );

  _Scheduler_Release_job( owner, deadline );

  _Thread_Dispatch_enable( cpu_self );
}

void _Rate_monotonic_Renew_deadline(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
)
{
  /*KHCHEN 03.08, Only renew the deadline called by the watchdog when it is TIMEOUT*/
  Per_CPU_Control *cpu_self;
  uint64_t deadline;

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( owner, lock_context );

  _ISR_lock_ISR_disable( lock_context );
  deadline = _Watchdog_Per_CPU_insert_relative(
    &the_period->Timer,
    cpu_self,
    the_period->next_length
  );
  the_period->latest_deadline = deadline;
  _ISR_lock_ISR_enable( lock_context );
  _Thread_Dispatch_enable( cpu_self );

}

void _Rate_monotonic_Restart(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
)
{
  /*
   *  Set the starting point and the CPU time used for the statistics.
   */
  _TOD_Get_uptime( &the_period->time_period_initiated );
  _Thread_Get_CPU_time_used( owner, &the_period->cpu_usage_period_initiated );

  _Rate_monotonic_Release_job(
    the_period,
    owner,
    the_period->next_length,
    lock_context
  );
}

static void _Rate_monotonic_Update_statistics(
  Rate_monotonic_Control    *the_period
)
{
  Timestamp_Control          executed;
  Timestamp_Control          since_last_period;
  Rate_monotonic_Statistics *stats;
  bool                       valid_status;

  /*
   *  Assume we are only called in states where it is appropriate
   *  to update the statistics.  This should only be RATE_MONOTONIC_ACTIVE
   *  and RATE_MONOTONIC_EXPIRED.
   */

  /*
   *  Update the counts.
   */
  stats = &the_period->Statistics;
  stats->count++;

  if ( the_period->state == RATE_MONOTONIC_EXPIRED )
    stats->missed_count++;

  /*
   *  Grab status for time statistics.
   */
  valid_status =
    _Rate_monotonic_Get_status( the_period, &since_last_period, &executed );
  if (!valid_status)
    return;

  /*
   *  Update CPU time
   */
  _Timestamp_Add_to( &stats->total_cpu_time, &executed );

  if ( _Timestamp_Less_than( &executed, &stats->min_cpu_time ) )
    stats->min_cpu_time = executed;

  if ( _Timestamp_Greater_than( &executed, &stats->max_cpu_time ) )
    stats->max_cpu_time = executed;

  /*
   *  Update Wall time
   */
  _Timestamp_Add_to( &stats->total_wall_time, &since_last_period );

  if ( _Timestamp_Less_than( &since_last_period, &stats->min_wall_time ) )
    stats->min_wall_time = since_last_period;

  if ( _Timestamp_Greater_than( &since_last_period, &stats->max_wall_time ) )
    stats->max_wall_time = since_last_period;
}

static rtems_status_code _Rate_monotonic_Get_status_for_state(
  rtems_rate_monotonic_period_states state
)
{
  switch ( state ) {
    case RATE_MONOTONIC_INACTIVE:
      return RTEMS_NOT_DEFINED;
    case RATE_MONOTONIC_EXPIRED:
      return RTEMS_TIMEOUT;
    default:
      _Assert( state == RATE_MONOTONIC_ACTIVE );
      return RTEMS_SUCCESSFUL;
  }
}

static rtems_status_code _Rate_monotonic_Activate(
  Rate_monotonic_Control *the_period,
  rtems_interval          length,
  Thread_Control         *executing,
  ISR_lock_Context       *lock_context
)
{
  /** KHCHEN 02.08
   * Initialize the # of postponed job variable */
  the_period->postponed_jobs = 0;

  the_period->state = RATE_MONOTONIC_ACTIVE;
  the_period->next_length = length;
  _Rate_monotonic_Restart( the_period, executing, lock_context );
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code _Rate_monotonic_Block_while_active(
  Rate_monotonic_Control *the_period,
  rtems_interval          length,
  Thread_Control         *executing,
  ISR_lock_Context       *lock_context
)
{
  Per_CPU_Control *cpu_self;
  bool             success;

  /*
   *  Update statistics from the concluding period.
   */
  _Rate_monotonic_Update_statistics( the_period );

  /*
   *  This tells the _Rate_monotonic_Timeout that this task is
   *  in the process of blocking on the period and that we
   *  may be changing the length of the next period.
   */
  the_period->next_length = length;
  executing->Wait.return_argument = the_period;
  _Thread_Wait_flags_set( executing, RATE_MONOTONIC_INTEND_TO_BLOCK );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( executing, lock_context );

  _Thread_Set_state( executing, STATES_WAITING_FOR_PERIOD );

  success = _Thread_Wait_flags_try_change_acquire(
    executing,
    RATE_MONOTONIC_INTEND_TO_BLOCK,
    RATE_MONOTONIC_BLOCKED
  );
  if ( !success ) {
    _Assert(
      _Thread_Wait_flags_get( executing ) == RATE_MONOTONIC_READY_AGAIN
    );
    _Thread_Unblock( executing );
  }

  _Thread_Dispatch_enable( cpu_self );
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code _Rate_monotonic_Block_while_expired(
  Rate_monotonic_Control *the_period,
  rtems_interval          length,
  Thread_Control         *executing,
  ISR_lock_Context       *lock_context
)
{
  /* There are two possible cases, one is that the previous deadline is missed, 
   * i.e., the state = RATE_MONOTONIC_EXPIRED. The other is that the # of postponed jobs is not 0,
   * but the current deadline is still not expired, i.e., state = RATE_MONOTONIC_ACTIVE.
   * However, only when the # of postponed jobs is larger than 0, we will reach this Block.
   * It means, the just finished job is not the exact job released in the current period. Even if 
   * they are in time, they are not in the proper periods they belong to.   */

  /* no matter the just finished jobs in time or not, 
   * they are actually missing their deadlines already. */
  if(the_period->postponed_jobs > 1)
    the_period->state = RATE_MONOTONIC_EXPIRED;
  /*
   *  Update statistics from the concluding period
   */
  _Rate_monotonic_Update_statistics( the_period );

  the_period->state = RATE_MONOTONIC_ACTIVE;
  the_period->next_length = length;

  //KHCHEN 02.08
//  _Rate_monotonic_Release_job( the_period, executing, length, lock_context );
  _Rate_monotonic_Release_postponedjob( the_period, executing, length, lock_context );
  return RTEMS_TIMEOUT;
}

uint32_t _Rate_monotonic_Postponed_num(
    rtems_id   period_id
)
{
  /* This is a helper function to return the number of postponed jobs in the given period. */
  Rate_monotonic_Control             *the_period;
  ISR_lock_Context                    lock_context;
  Thread_Control                     *owner;

  the_period = _Rate_monotonic_Get( period_id, &lock_context );
  _Assert(the_period != NULL);
  uint32_t jobs = the_period->postponed_instances;
  owner = the_period->owner;
  _Rate_monotonic_Release( owner, &lock_context );
  return jobs;
}

rtems_status_code rtems_rate_monotonic_period(
  rtems_id       id,
  rtems_interval length
)
{
  Rate_monotonic_Control            *the_period;
  ISR_lock_Context                   lock_context;
  Thread_Control                    *executing;
  rtems_status_code                  status;
  rtems_rate_monotonic_period_states state;

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  executing = _Thread_Executing;
  if ( executing != the_period->owner ) {
    _ISR_lock_ISR_enable( &lock_context );
    return RTEMS_NOT_OWNER_OF_RESOURCE;
  }

  _Rate_monotonic_Acquire_critical( executing, &lock_context );

  state = the_period->state;

  if ( length == RTEMS_PERIOD_STATUS ) {
    status = _Rate_monotonic_Get_status_for_state( state );
    _Rate_monotonic_Release( executing, &lock_context );
  } else {
    switch ( state ) {
      case RATE_MONOTONIC_ACTIVE:

        if(the_period->postponed_jobs > 0){
          /* KHCHEN 03.08
           * If there is no timeout in this period but the number of postponed
           * jobs is not 0, it means the previous postponed instance is
           * finished without exceeding the current period deadline. 
           *
           * Do nothing on the watchdog deadline assignment but release the next 
           * remaining postponed job.
           */ 
          
          status = _Rate_monotonic_Block_while_expired(
            the_period,
            length,
            executing,
            &lock_context
          );
        }else{
          /* KHCHEN 03.08
           * Normal case that no postponed jobs and no expiration, so wait for the period 
           * and update the deadline of watchdog accordingly.
           */
          
          status = _Rate_monotonic_Block_while_active(
            the_period,
            length,
            executing,
            &lock_context
          );
        }
        break;
      case RATE_MONOTONIC_INACTIVE:
        status = _Rate_monotonic_Activate(
          the_period,
          length,
          executing,
          &lock_context
        );
        break;
      default:
        /** KHCHEN 03.08
         * As now this period was already TIMEOUT, there must be at least one 
         * postponed job recorded by the watchdog. The one which exceeded
         * the previous deadline"s" was just finished.
         * 
         * Maybe there is more than one job postponed due to the preemption or
         * the previous finished job.
         */

        _Assert( state == RATE_MONOTONIC_EXPIRED );
        status = _Rate_monotonic_Block_while_expired(
          the_period,
          length,
          executing,
          &lock_context
        );
        break;
    }
  }

  return status;
}
