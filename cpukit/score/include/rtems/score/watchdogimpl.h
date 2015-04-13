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
#include <rtems/score/chainimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreWatchdog
 *  @{
 */

/**
 *  @brief Control block used to manage intervals.
 *
 *  The following type defines the control block used to manage
 *  intervals.
 */
#define WATCHDOG_MAXIMUM_INTERVAL ((Watchdog_Interval) 0xffffffff)

/**
 * @brief Watchdog initializer for static initialization.
 *
 * @see _Watchdog_Initialize().
 */
#define WATCHDOG_INITIALIZER( routine, id, user_data ) \
  { \
    { NULL, NULL }, \
    WATCHDOG_INACTIVE, \
    0, 0, 0, 0, \
    ( routine ), ( id ), ( user_data ) \
  }

/**
 * @brief Watchdog header.
 */
typedef struct {
  /**
   * @brief The chain of active or transient watchdogs.
   */
  Chain_Control Watchdogs;
} Watchdog_Header;

/**
 *  @brief Watchdog synchronization level.
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_level;

/**
 *  @brief Watchdog synchronization count.
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_count;

/**
 *  @brief Watchdog chain which is managed at ticks.
 *
 *  This is the watchdog chain which is managed at ticks.
 */
SCORE_EXTERN Watchdog_Header _Watchdog_Ticks_header;

/**
 *  @brief Watchdog chain which is managed at second boundaries.
 *
 *  This is the watchdog chain which is managed at second boundaries.
 */
SCORE_EXTERN Watchdog_Header _Watchdog_Seconds_header;

/**
 *  @brief Initialize the watchdog handler.
 *
 *  This routine initializes the watchdog handler.  The watchdog
 *  synchronization flag is initialized and the watchdog chains are
 *  initialized and emptied.
 */
void _Watchdog_Handler_initialization( void );

/**
 *  @brief Removes @a the_watchdog from the watchdog chain.
 *
 *  This routine removes @a the_watchdog from the watchdog chain on which
 *  it resides and returns the state @a the_watchdog timer was in.
 *
 *  @param[in] the_watchdog will be removed
 *  @retval the state in which @a the_watchdog was in when removed
 */
Watchdog_States _Watchdog_Remove (
  Watchdog_Control *the_watchdog
);

/**
 *  @brief Adjusts the header watchdog chain in the backward direction for
 *  units ticks.
 *
 *  @param[in] header The watchdog chain.
 *  @param[in] units The units of ticks to adjust.
 */
void _Watchdog_Adjust_backward(
  Watchdog_Header   *header,
  Watchdog_Interval  units
);

/**
 *  @brief Adjusts the header watchdog chain in the forward direction for units
 *  ticks.
 *
 *  This may lead to several _Watchdog_Tickle() invocations.
 *
 *  @param[in] header The watchdog chain.
 *  @param[in] units The units of ticks to adjust.
 */
void _Watchdog_Adjust_forward(
  Watchdog_Header   *header,
  Watchdog_Interval  units
);

/**
 *  @brief Adjusts the @a header watchdog chain in the forward
 *  @a direction for @a units_arg ticks.
 *
 *  This routine adjusts the @a header watchdog chain in the forward
 *  @a direction for @a units_arg ticks.
 *
 *  @param[in] header is the watchdog chain to adjust
 *  @param[in] units_arg is the number of units to adjust @a header
 *  @param[in] to_fire is a pointer to an initialized Chain_Control to which
 *             all watchdog instances that are to be fired will be placed.
 *
 *  @note This always adjusts forward.
 */
void _Watchdog_Adjust_to_chain(
  Watchdog_Header   *header,
  Watchdog_Interval  units_arg,
  Chain_Control     *to_fire

);

/**
 *  @brief Inserts @a the_watchdog into the @a header watchdog chain
 *  for a time of @a units.
 *
 *  This routine inserts @a the_watchdog into the @a header watchdog chain
 *  for a time of @a units.
 *  Update the delta interval counters.
 *
 *  @param[in] header is @a the_watchdog list to insert @a the_watchdog on
 *  @param[in] the_watchdog is the watchdog to insert
 */
void _Watchdog_Insert (
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
);

/**
 *  @brief This routine is invoked at appropriate intervals to update
 *  the @a header watchdog chain.
 *
 *  This routine is invoked at appropriate intervals to update
 *  the @a header watchdog chain.
 *  This routine decrements the delta counter in response to a tick.
 *
 *  @param[in] header is the watchdog chain to tickle
 */
void _Watchdog_Tickle (
  Watchdog_Header *header
);

/**
 * This routine initializes the specified watchdog.  The watchdog is
 * made inactive, the watchdog id and handler routine are set to the
 * specified values.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Initialize(
  Watchdog_Control               *the_watchdog,
  Watchdog_Service_routine_entry  routine,
  Objects_Id                      id,
  void                           *user_data
)
{
  the_watchdog->state     = WATCHDOG_INACTIVE;
  the_watchdog->routine   = routine;
  the_watchdog->id        = id;
  the_watchdog->user_data = user_data;
}

/**
 * This routine returns true if the watchdog timer is in the ACTIVE
 * state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_active(
  Watchdog_Control *the_watchdog
)
{

  return ( the_watchdog->state == WATCHDOG_ACTIVE );

}

/**
 * This routine activates THE_WATCHDOG timer which is already
 * on a watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Activate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_ACTIVE;

}

/**
 * This routine deactivates THE_WATCHDOG timer which will remain
 * on a watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Deactivate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_REMOVE_IT;

}

/**
 * This routine is invoked at each clock tick to update the ticks
 * watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_ticks( void )
{

  _Watchdog_Tickle( &_Watchdog_Ticks_header );

}

/**
 * This routine is invoked at each clock tick to update the seconds
 * watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_seconds( void )
{

  _Watchdog_Tickle( &_Watchdog_Seconds_header );

}

/**
 * This routine inserts THE_WATCHDOG into the ticks watchdog chain
 * for a time of UNITS ticks.  The INSERT_MODE indicates whether
 * THE_WATCHDOG is to be activated automatically or later, explicitly
 * by the caller.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Insert_ticks(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Ticks_header, the_watchdog );

}

/**
 * This routine inserts THE_WATCHDOG into the seconds watchdog chain
 * for a time of UNITS seconds.  The INSERT_MODE indicates whether
 * THE_WATCHDOG is to be activated automatically or later, explicitly
 * by the caller.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Insert_seconds(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Seconds_header, the_watchdog );

}

/**
 * This routine resets THE_WATCHDOG timer to its state at INSERT
 * time.  This routine is valid only on interval watchdog timers
 * and is used to make an interval watchdog timer fire "every" so
 * many ticks.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Reset_ticks(
  Watchdog_Control *the_watchdog
)
{

  (void) _Watchdog_Remove( the_watchdog );

  _Watchdog_Insert( &_Watchdog_Ticks_header, the_watchdog );

}

/**
 * This routine returns a pointer to the watchdog timer following
 * THE_WATCHDOG on the watchdog chain.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Next(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.next );

}

/**
 * This routine returns a pointer to the watchdog timer preceding
 * THE_WATCHDOG on the watchdog chain.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Previous(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.previous );

}

/**
 * This routine returns a pointer to the first watchdog timer
 * on the watchdog chain HEADER.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_First(
  Watchdog_Header *header
)
{

  return ( (Watchdog_Control *) _Chain_First( &header->Watchdogs ) );

}

/**
 * This routine returns a pointer to the last watchdog timer
 * on the watchdog chain HEADER.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Last(
  Watchdog_Header *header
)
{

  return ( (Watchdog_Control *) _Chain_Last( &header->Watchdogs ) );

}

RTEMS_INLINE_ROUTINE bool _Watchdog_Is_empty(
  const Watchdog_Header *header
)
{
  return _Chain_Is_empty( &header->Watchdogs );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Header_initialize(
  Watchdog_Header *header
)
{
  _Chain_Initialize_empty( &header->Watchdogs );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
