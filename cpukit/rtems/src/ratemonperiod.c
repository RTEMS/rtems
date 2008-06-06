/*
 *  Rate Monotonic Manager - Period Blocking and Status
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>
#if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS) || \
    defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
  #include <rtems/score/timespec.h>
  extern struct timespec _Thread_Time_of_last_context_switch;
#endif

void _Rate_monotonic_Update_statistics(
  Rate_monotonic_Control    *the_period
)
{
  rtems_rate_monotonic_period_statistics *stats;
  rtems_thread_cpu_usage_t                executed;
  rtems_rate_monotonic_period_time_t      since_last_period;
  #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    rtems_rate_monotonic_period_time_t    period_start;
  #endif
  #if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS) || \
      defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
    struct timespec  uptime;

    /*
     * Obtain the current time since boot
     */
    _TOD_Get_uptime( &uptime );
  #endif

  /*
   *  Assume we are only called in states where it is appropriate
   *  to update the statistics.  This should only be RATE_MONOTONIC_ACTIVE
   *  and RATE_MONOTONIC_EXPIRED.
   */

  /*
   *  Grab basic information
   */

  #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    period_start               = the_period->time_at_period;
    _Timespec_Subtract( &period_start, &uptime, &since_last_period );
    the_period->time_at_period = uptime;
  #else
    since_last_period = _Watchdog_Ticks_since_boot - the_period->time_at_period;
    the_period->time_at_period = _Watchdog_Ticks_since_boot;
  #endif

  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    {
      rtems_rate_monotonic_period_time_t ran, used;
       
      /* Grab CPU usage when the thread got switched in */
      used = _Thread_Executing->cpu_time_used;

      /* How much time time since last context switch */
      _Timespec_Subtract(&_Thread_Time_of_last_context_switch, &uptime, &ran);

      /* executed += ran */
      _Timespec_Add_to( &used, &ran );

       /* executed = current cpu usage - value at start of period */
      _Timespec_Subtract( 
         &the_period->owner_executed_at_period,
         &used,
         &executed
      );
    }
  #else
      executed = the_period->owner->cpu_time_used -
        the_period->owner_executed_at_period;
  #endif

  /*
   *  Now update the statistics
   */

  stats = &the_period->Statistics;
  stats->count++;


  if ( the_period->state == RATE_MONOTONIC_EXPIRED )
    stats->missed_count++;

  /*
   *  Update CPU time
   */

  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    _Timespec_Add_to( &stats->total_cpu_time, &executed );

    if ( _Timespec_Less_than( &executed, &stats->min_cpu_time ) ) 
      stats->min_cpu_time = executed;

    if ( _Timespec_Greater_than( &executed, &stats->max_cpu_time ) ) 
      stats->max_cpu_time = executed;
  #else
    stats->total_cpu_time  += executed;

    if ( executed < stats->min_cpu_time )
      stats->min_cpu_time = executed;

    if ( executed > stats->max_cpu_time )
      stats->max_cpu_time = executed;
  #endif

  /*
   *  Update Wall time
   */

  #ifndef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    stats->total_wall_time += since_last_period;

    if ( since_last_period < stats->min_wall_time )
      stats->min_wall_time = since_last_period;

    if ( since_last_period > stats->max_wall_time )
      stats->max_wall_time = since_last_period;
  #else
    _Timespec_Add_to( &stats->total_wall_time, &since_last_period );

    if ( _Timespec_Less_than( &since_last_period, &stats->min_wall_time ) )
      stats->min_wall_time = since_last_period;

    if ( _Timespec_Greater_than( &since_last_period, &stats->max_wall_time ) )
      stats->max_wall_time = since_last_period;
  #endif
}


/*PAGE
 *
 *  rtems_rate_monotonic_period
 *
 *  This directive allows a thread to manipulate a rate monotonic timer.
 *
 *  Input parameters:
 *    id     - rate monotonic id
 *    length - length of period (in ticks)
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_period(
  Objects_Id        id,
  rtems_interval    length
)
{
  Rate_monotonic_Control              *the_period;
  Objects_Locations                    location;
  rtems_status_code                    return_value;
  rtems_rate_monotonic_period_states   local_state;
  ISR_Level                            level;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( !_Thread_Is_executing( the_period->owner ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_NOT_OWNER_OF_RESOURCE;
      }

      if ( length == RTEMS_PERIOD_STATUS ) {
        switch ( the_period->state ) {
          case RATE_MONOTONIC_INACTIVE:
            return_value = RTEMS_NOT_DEFINED;
            break;
          case RATE_MONOTONIC_EXPIRED:
          case RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING:
            return_value = RTEMS_TIMEOUT;
            break;
          case RATE_MONOTONIC_ACTIVE:
          default:              /* unreached -- only to remove warnings */
            return_value = RTEMS_SUCCESSFUL;
            break;
        }
        _Thread_Enable_dispatch();
        return( return_value );
      }

      _ISR_Disable( level );
      switch ( the_period->state ) {
        case RATE_MONOTONIC_INACTIVE: {
          #if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS) || \
              defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
            struct timespec uptime;
          #endif

          /*
           *  No need to update statistics -- there are not a period active
           */

          _ISR_Enable( level );


          #if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS) || \
              defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
            _TOD_Get_uptime( &uptime );
          #endif
              
          #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
            /*
             * Since the statistics didn't update the starting time,
             * we do it here.
             */
            the_period->time_at_period = uptime;
          #else
            the_period->time_at_period = _Watchdog_Ticks_since_boot;
          #endif

          #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
            { 
              rtems_thread_cpu_usage_t ran;

              the_period->owner_executed_at_period = 
                _Thread_Executing->cpu_time_used;

              /* How much time time since last context switch */
              _Timespec_Subtract(
                &_Thread_Time_of_last_context_switch,
                &uptime,
                &ran
              );

              /* The thread had executed before the last context switch also.
               *   
               *     the_period->owner_executed_at_period += ran
               */
              _Timespec_Add_to( &the_period->owner_executed_at_period, &ran );
            }
          #else
            the_period->owner_executed_at_period =
              _Thread_Executing->cpu_time_used;
          #endif

          the_period->state = RATE_MONOTONIC_ACTIVE;
          _Watchdog_Initialize(
            &the_period->Timer,
            _Rate_monotonic_Timeout,
            id,
            NULL
          );

          the_period->next_length = length;

          _Watchdog_Insert_ticks( &the_period->Timer, length );
          _Thread_Enable_dispatch();
          return RTEMS_SUCCESSFUL;
        }
        case RATE_MONOTONIC_ACTIVE:

          /*
           *  Update statistics from the concluding period
           */
          _Rate_monotonic_Update_statistics( the_period );

          /*
           *  This tells the _Rate_monotonic_Timeout that this task is
           *  in the process of blocking on the period and that we
           *  may be changing the length of the next period.
           */

          the_period->state = RATE_MONOTONIC_OWNER_IS_BLOCKING;
          the_period->next_length = length;

          _ISR_Enable( level );

          _Thread_Executing->Wait.id = the_period->Object.id;
          _Thread_Set_state( _Thread_Executing, STATES_WAITING_FOR_PERIOD );

          /*
           *  Did the watchdog timer expire while we were actually blocking
           *  on it?
           */

          _ISR_Disable( level );
            local_state = the_period->state;
            the_period->state = RATE_MONOTONIC_ACTIVE;
          _ISR_Enable( level );

          /*
           *  If it did, then we want to unblock ourself and continue as
           *  if nothing happen.  The period was reset in the timeout routine.
           */

          if ( local_state == RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING )
            _Thread_Clear_state( _Thread_Executing, STATES_WAITING_FOR_PERIOD );

          _Thread_Enable_dispatch();
          return RTEMS_SUCCESSFUL;
          break;

        case RATE_MONOTONIC_EXPIRED:
          /*
           *  Update statistics from the concluding period
           */
          _Rate_monotonic_Update_statistics( the_period );

          _ISR_Enable( level );

          the_period->state = RATE_MONOTONIC_ACTIVE;
          the_period->next_length = length;

          _Watchdog_Insert_ticks( &the_period->Timer, length );
          _Thread_Enable_dispatch();
          return RTEMS_TIMEOUT;

        case RATE_MONOTONIC_OWNER_IS_BLOCKING:
        case RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING:
          /*
           *  These should never happen.
           */
          break;
      }

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
