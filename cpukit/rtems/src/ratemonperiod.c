/*
 *  Rate Monotonic Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

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
 *    error code        - if unsuccessful
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
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

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
          case RATE_MONOTONIC_ACTIVE:
            return_value = RTEMS_SUCCESSFUL;
            break;
          case RATE_MONOTONIC_EXPIRED:
            return_value = RTEMS_TIMEOUT;
            break;
          default:              /* unreached -- only to remove warnings */
            return_value = RTEMS_INTERNAL_ERROR;
            break;
        }
        _Thread_Enable_dispatch();
        return( return_value );
      }

      _ISR_Disable( level );
      switch ( the_period->state ) {
        case RATE_MONOTONIC_INACTIVE:
          _ISR_Enable( level );
          the_period->state = RATE_MONOTONIC_ACTIVE;
          _Watchdog_Initialize(
            &the_period->Timer,
            _Rate_monotonic_Timeout,
            id,
            NULL
          );

          the_period->owner_ticks_executed_at_period =
            _Thread_Executing->ticks_executed;

          the_period->time_at_period = _Watchdog_Ticks_since_boot;
          the_period->next_length = length;

          _Watchdog_Insert_ticks( &the_period->Timer, length );
          _Thread_Enable_dispatch();
          return RTEMS_SUCCESSFUL;

        case RATE_MONOTONIC_ACTIVE:
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
          _ISR_Enable( level );
          the_period->state = RATE_MONOTONIC_ACTIVE;
          the_period->owner_ticks_executed_at_period =
            _Thread_Executing->ticks_executed;
          the_period->time_at_period = _Watchdog_Ticks_since_boot;
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
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
