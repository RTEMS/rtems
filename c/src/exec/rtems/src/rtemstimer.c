/*
 *  Timer Manager
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
#include <rtems/object.h>
#include <rtems/thread.h>
#include <rtems/timer.h>
#include <rtems/tod.h>
#include <rtems/watchdog.h>

/*PAGE
 *
 *  _Timer_Manager_initialization
 *
 *  This routine initializes all timer manager related data structures.
 *
 *  Input parameters:
 *    maximum_timers - number of timers to initialize
 *
 *  Output parameters:  NONE
 */

void _Timer_Manager_initialization(
  unsigned32 maximum_timers
)
{
  _Objects_Initialize_information(
    &_Timer_Information,
    OBJECTS_RTEMS_TIMERS,
    FALSE,
    maximum_timers,
    sizeof( Timer_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH
  );
}

/*PAGE
 *
 *  rtems_timer_create
 *
 *  This directive creates a timer and performs some initialization.
 *
 *  Input parameters:
 *    name - timer name
 *    id   - pointer to timer id
 *
 *  Output parameters:
 *    id                - timer id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_timer_create(
  rtems_name    name,
  Objects_Id   *id
)
{
  Timer_Control *the_timer;

  if ( !rtems_is_name_valid( name ) )
    return ( RTEMS_INVALID_NAME );

  _Thread_Disable_dispatch();         /* to prevent deletion */

  the_timer = _Timer_Allocate();

  if ( !the_timer ) {
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  the_timer->the_class = TIMER_DORMANT;

  _Objects_Open( &_Timer_Information, &the_timer->Object, &name );

  *id = the_timer->Object.id;
  _Thread_Enable_dispatch();
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_timer_ident
 *
 *  This directive returns the system ID associated with
 *  the timer name.
 *
 *  Input parameters:
 *    name - user defined message queue name
 *    id   - pointer to timer id
 *
 *  Output parameters:
 *    *id               - message queue id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_timer_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  return _Objects_Name_to_id(
    &_Timer_Information,
    &name,
    RTEMS_SEARCH_LOCAL_NODE,
    id
  );
}

/*PAGE
 *
 *  rtems_timer_cancel
 *
 *  This directive allows a thread to cancel a timer.
 *
 *  Input parameters:
 *    id - timer id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_timer_cancel(
  Objects_Id id
)
{
  Timer_Control   *the_timer;
  Objects_Locations       location;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      if ( !_Timer_Is_dormant_class( the_timer->the_class ) ) 
        (void) _Watchdog_Remove( &the_timer->Ticker );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_timer_delete
 *
 *  This directive allows a thread to delete a timer.
 *
 *  Input parameters:
 *    id - timer id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_timer_delete(
  Objects_Id id
)
{
  Timer_Control   *the_timer;
  Objects_Locations       location;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      _Objects_Close( &_Timer_Information, &the_timer->Object );
      (void) _Watchdog_Remove( &the_timer->Ticker );
      _Timer_Free( the_timer );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_timer_fire_after
 *
 *  This directive allows a thread to start a timer.
 *
 *  Input parameters:
 *    id      - timer id
 *    ticks   - interval until routine is fired
 *    routine - routine to schedule
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_timer_fire_after(
  Objects_Id         id,
  rtems_interval  ticks,
  Timer_Service      routine,
  void              *user_data
)
{
  Timer_Control   *the_timer;
  Objects_Locations       location;

  if ( ticks == 0 )
    return( RTEMS_INVALID_NUMBER );

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      (void) _Watchdog_Remove( &the_timer->Ticker );
      the_timer->the_class = TIMER_INTERVAL;
      _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
      _Watchdog_Insert_ticks( &the_timer->Ticker,
                                 ticks, WATCHDOG_ACTIVATE_NOW );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_timer_fire_when
 *
 *  This directive allows a thread to start a timer.
 *
 *  Input parameters:
 *    id        - timer id
 *    wall_time - time of day to fire timer
 *    routine   - routine to schedule
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_timer_fire_when(
  Objects_Id          id,
  rtems_time_of_day        *wall_time,
  Timer_Service       routine,
  void               *user_data
)
{
  Timer_Control   *the_timer;
  Objects_Locations       location;
  rtems_status_code            validate_status;
  rtems_interval       seconds;

  if ( !_TOD_Is_set() )
    return( RTEMS_NOT_DEFINED );

  validate_status = _TOD_Validate( wall_time );
  if ( !rtems_is_status_successful( validate_status ) )
    return( validate_status );

  seconds = _TOD_To_seconds( wall_time );
  if ( seconds <= _TOD_Seconds_since_epoch )
    return( RTEMS_INVALID_CLOCK );

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      (void) _Watchdog_Remove( &the_timer->Ticker );
      the_timer->the_class = TIMER_TIME_OF_DAY;
      _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
      _Watchdog_Insert_seconds( &the_timer->Ticker,
                seconds - _TOD_Seconds_since_epoch, WATCHDOG_ACTIVATE_NOW );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_timer_reset
 *
 *  This directive allows a thread to reset a timer.
 *
 *  Input parameters:
 *    id - timer id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_timer_reset(
  Objects_Id id
)
{
  Timer_Control *the_timer;
  Objects_Locations     location;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:            /* should never return this */
      return( RTEMS_INTERNAL_ERROR );
    case OBJECTS_LOCAL:
      if ( _Timer_Is_interval_class( the_timer->the_class ) ) {
        _Watchdog_Reset( &the_timer->Ticker );
        _Thread_Enable_dispatch();
        return( RTEMS_SUCCESSFUL );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_NOT_DEFINED );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}
