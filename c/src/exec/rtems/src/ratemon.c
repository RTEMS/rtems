/*
 *  Rate Monotonic Manager
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  _Rate_monotonic_Manager_initialization
 *
 *  This routine initializes all Rate Monotonic Manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_periods - number of periods timers to initialize
 *
 *  Output parameters:  NONE
 *
 *  NOTE: The Rate Monotonic Manager is built on top of the Watchdog
 *        Handler.
 */

void _Rate_monotonic_Manager_initialization(
  unsigned32 maximum_periods
)
{
  _Objects_Initialize_information(
    &_Rate_monotonic_Information,
    OBJECTS_RTEMS_PERIODS,
    FALSE,
    maximum_periods,
    sizeof( Rate_monotonic_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );
}

/*PAGE
 *
 *  rtems_rate_monotonic_create
 *
 *  This directive creates a rate monotonic timer and performs
 *  some initialization.
 *
 *  Input parameters:
 *    name - name of period
 *    id   - pointer to rate monotonic id
 *
 *  Output parameters:
 *    id                - rate monotonic id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_create(
  rtems_name    name,
  Objects_Id   *id
)
{
  Rate_monotonic_Control *the_period;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  _Thread_Disable_dispatch();            /* to prevent deletion */

  the_period = _Rate_monotonic_Allocate();

  if ( !the_period ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_period->owner = _Thread_Executing;
  the_period->state = RATE_MONOTONIC_INACTIVE;

  _Objects_Open( &_Rate_monotonic_Information, &the_period->Object, &name );

  *id = the_period->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_rate_monotonic_ident
 *
 *  This directive returns the system ID associated with
 *  the rate monotonic period name.
 *
 *  Input parameters:
 *    name - user defined period name
 *    id   - pointer to period id
 *
 *  Output parameters:
 *    *id               - region id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Rate_monotonic_Information,
    &name,
    OBJECTS_SEARCH_LOCAL_NODE,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}

/*PAGE
 *
 *  rtems_rate_monotonic_cancel
 *
 *  This directive allows a thread to cancel a rate monotonic timer.
 *
 *  Input parameters:
 *    id - rate monotonic id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful and caller is not the owning thread
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_cancel(
  Objects_Id id
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations              location;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:            
      return RTEMS_INTERNAL_ERROR;  /* should never return this */ 
    case OBJECTS_LOCAL:
      if ( !_Thread_Is_executing( the_period->owner ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_NOT_OWNER_OF_RESOURCE;
      }
      (void) _Watchdog_Remove( &the_period->Timer );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_rate_monotonic_delete
 *
 *  This directive allows a thread to delete a rate monotonic timer.
 *
 *  Input parameters:
 *    id - rate monotonic id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_delete(
  Objects_Id id
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations              location;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:
      _Objects_Close( &_Rate_monotonic_Information, &the_period->Object );
      (void) _Watchdog_Remove( &the_period->Timer );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Rate_monotonic_Free( the_period );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_rate_monotonic_get_status
 *
 *  This directive allows a thread to obtain status information on a 
 *  period.
 *
 *  Input parameters:
 *    id     - rate monotonic id
 *    status - pointer to status control block
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 *
 */

rtems_status_code rtems_rate_monotonic_get_status(
  Objects_Id                           id,
  rtems_rate_monotonic_period_status  *status
)
{
  Objects_Locations              location;
  Rate_monotonic_Control        *the_period;

  if ( status == NULL )
    return RTEMS_INVALID_ADDRESS;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;
    case OBJECTS_LOCAL:
      status->state = the_period->state;

      if ( status->state == RATE_MONOTONIC_INACTIVE ) {
        status->ticks_since_last_period = 0;
        status->ticks_executed_since_last_period = 0;
      } else {
        status->ticks_since_last_period =
          _Watchdog_Ticks_since_boot - the_period->time_at_period;

        status->ticks_executed_since_last_period =
          the_period->owner->ticks_executed -
            the_period->owner_ticks_executed_at_period;
      }

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;
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

          _Watchdog_Insert_ticks( &the_period->Timer, length );
          _Thread_Enable_dispatch();
          return RTEMS_SUCCESSFUL;

        case RATE_MONOTONIC_ACTIVE:
          /*
           *  This tells the _Rate_monotonic_Timeout that this task is
           *  in the process of blocking on the period.
           */

          the_period->state = RATE_MONOTONIC_OWNER_IS_BLOCKING;
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

/*PAGE
 *
 *  _Rate_monotonic_Timeout
 *
 *  This routine processes a period ending.  If the owning thread
 *  is waiting for the period, that thread is unblocked and the
 *  period reinitiated.  Otherwise, the period is expired.
 *  This routine is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - period id
 *
 *  Output parameters: NONE
 */

void _Rate_monotonic_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations       location;
  Thread_Control         *the_thread;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      the_thread = the_period->owner;
      if ( _States_Is_waiting_for_period( the_thread->current_state ) &&
            the_thread->Wait.id == the_period->Object.id ) {
        _Thread_Unblock( the_thread );
        the_period->owner_ticks_executed_at_period =
          the_thread->ticks_executed;

        the_period->time_at_period = _Watchdog_Ticks_since_boot;

        _Watchdog_Reset( &the_period->Timer );
      } else if ( the_period->state == RATE_MONOTONIC_OWNER_IS_BLOCKING ) {
        the_period->state = RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING;
        the_period->owner_ticks_executed_at_period =
          the_thread->ticks_executed;

        the_period->time_at_period = _Watchdog_Ticks_since_boot;
        _Watchdog_Reset( &the_period->Timer );
      } else
        the_period->state = RATE_MONOTONIC_EXPIRED;
      _Thread_Unnest_dispatch();
      break;
  }
}

