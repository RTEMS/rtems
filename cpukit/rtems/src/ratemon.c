/*
 *  Rate Monotonic Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/support.h>
#include <rtems/isr.h>
#include <rtems/object.h>
#include <rtems/ratemon.h>
#include <rtems/thread.h>

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
    RTEMS_MAXIMUM_NAME_LENGTH
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
    return( RTEMS_INVALID_NAME );

  _Thread_Disable_dispatch();            /* to prevent deletion */

  the_period = _Rate_monotonic_Allocate();

  if ( !the_period ) {
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  the_period->owner = _Thread_Executing;
  the_period->state = RATE_MONOTONIC_INACTIVE;

  _Objects_Open( &_Rate_monotonic_Information, &the_period->Object, &name );

  *id = the_period->Object.id;
  _Thread_Enable_dispatch();
  return( RTEMS_SUCCESSFUL );
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
  return _Objects_Name_to_id(
    &_Rate_monotonic_Information,
    &name,
    RTEMS_SEARCH_LOCAL_NODE,
    id
  );
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
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      if ( !_Thread_Is_executing( the_period->owner ) ) {
        _Thread_Enable_dispatch();
        return( RTEMS_NOT_OWNER_OF_RESOURCE );
      }
      (void) _Watchdog_Remove( &the_period->Timer );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
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
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      _Objects_Close( &_Rate_monotonic_Information, &the_period->Object );
      (void) _Watchdog_Remove( &the_period->Timer );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Rate_monotonic_Free( the_period );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
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
  rtems_interval length
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations       location;
  rtems_status_code            return_value;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      if ( !_Thread_Is_executing( the_period->owner ) ) {
        _Thread_Enable_dispatch();
        return( RTEMS_NOT_OWNER_OF_RESOURCE );
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

      switch ( the_period->state ) {
        case RATE_MONOTONIC_INACTIVE:
          the_period->state = RATE_MONOTONIC_ACTIVE;
          _Watchdog_Initialize(
            &the_period->Timer,
            _Rate_monotonic_Timeout,
            id,
            NULL
          );
          _Watchdog_Insert_ticks(
                     &the_period->Timer, length, WATCHDOG_ACTIVATE_NOW );
          _Thread_Enable_dispatch();
          return( RTEMS_SUCCESSFUL );

        case RATE_MONOTONIC_ACTIVE:
/* following is and could be a critical section problem */
          _Thread_Executing->Wait.id  = the_period->Object.id;
          if ( _Rate_monotonic_Set_state( the_period ) ) {
            _Thread_Enable_dispatch();
            return( RTEMS_SUCCESSFUL );
          }
         /* has expired -- fall into next case */
        case RATE_MONOTONIC_EXPIRED:
          the_period->state = RATE_MONOTONIC_ACTIVE;
          _Watchdog_Insert_ticks(
                     &the_period->Timer, length, WATCHDOG_ACTIVATE_NOW );
          _Thread_Enable_dispatch();
          return( RTEMS_TIMEOUT );
      }
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 * _Rate_monotonic_Set_state
 *
 * This kernel routine sets the STATES_WAITING_FOR_PERIOD state in
 * the running thread's tcb if the specified period has not expired.
 * The ready chain is adjusted if necessary.
 *
 * Input parameters:
 *   the_period - pointer to period control block
 *
 * Output parameters:
 *   TRUE  - if blocked successfully for period
 *   FALSE - if period has expired
 *
 *  INTERRUPT LATENCY:
 *    delete node
 *    priority map
 *    select heir
 */

boolean _Rate_monotonic_Set_state(
Rate_monotonic_Control *the_period
)
{
  Thread_Control *executing;
  Chain_Control  *ready;
  ISR_Level              level;
  States_Control         old_state;

  executing = _Thread_Executing;
  ready     = executing->ready;
  _ISR_Disable( level );

  old_state = executing->current_state;

  if ( _Rate_monotonic_Is_expired( the_period ) ) {
    _ISR_Enable( level );
    return( FALSE );
  }

  executing->current_state =
             _States_Set( STATES_WAITING_FOR_PERIOD, old_state );

  if ( _States_Is_ready( old_state ) ) {
    if ( _Chain_Has_only_one_node( ready ) ) {
      _Chain_Initialize_empty( ready );
      _Priority_Remove_from_bit_map( &executing->Priority_map );
      _ISR_Flash( level );
    } else {
      _Chain_Extract_unprotected( &executing->Object.Node );
      _ISR_Flash( level );
    }

    if ( _Thread_Is_heir( executing ) )
      _Thread_Calculate_heir();

    _Context_Switch_necessary = TRUE;
  }

  _ISR_Enable( level );
  return( TRUE );
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
  Objects_Locations              location;
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
        _Watchdog_Reset( &the_period->Timer );
      }
      else
        the_period->state = RATE_MONOTONIC_EXPIRED;
      _Thread_Unnest_dispatch();
      break;
  }
}

