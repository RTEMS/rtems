/** 
 *  @file  rtems/score/watchdog.h
 *
 *  This include file contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *  used to initialize and manipulate watchdog timers.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __WATCHDOG_h
#define __WATCHDOG_h

/**
 *  @defgroup ScoreWatchdog Watchdog Handler
 *
 *  This group contains functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>

/** @brief Maximum Interval Length
 *
 *  The following type defines the control block used to manage
 *  intervals.
 */
#define WATCHDOG_MAXIMUM_INTERVAL ((Watchdog_Interval) 0xffffffff)

/** @brief Watchdog Interval Type
 *
 *  This type is used to specify the length of intervals.
 */
typedef uint32_t   Watchdog_Interval;

/** @brief Watchdog Service Routine Return Type
 * 
 *  This type defines the return type from a Watchdog Service Routine.
 */
typedef void Watchdog_Service_routine;

/** @brief Watchdog Service Routine Pointer Type
 *
 *  This type define a pointer to a watchdog service routine.
 */
typedef Watchdog_Service_routine ( *Watchdog_Service_routine_entry )(
                 Objects_Id,
                 void *
             );

/** @brief No timeout constant
 *
 *  This is the constant for indefinite wait.  It is actually an
 *  illegal interval.
 */
#define WATCHDOG_NO_TIMEOUT  0

/** @brief Watchdog States Type
 *
 *  This enumerated type is the set of the states in which a
 *  watchdog timer may be at any given time.
 */

typedef enum {
  /** This is the state when the watchdog is off all chains */
  WATCHDOG_INACTIVE,
  /** This is the state when the watchdog is off all chains, but we are
   *  currently searching for the insertion point.
   */
  WATCHDOG_BEING_INSERTED,
  /** This is the state when the watchdog is on a chain, and allowed to fire. */
  WATCHDOG_ACTIVE,
  /** This is the state when the watchdog is on a chain, but we should
   *  remove without firing if it expires.
   */
  WATCHDOG_REMOVE_IT
} Watchdog_States;

/** @brief Watchdog Adjustment Directions Type
 *
 *  The following enumerated type details the manner in which
 *  a watchdog chain may be adjusted by the @ref _Watchdog_Adjust
 *  routine.  The direction indicates a movement FORWARD
 *  or BACKWARD in time.
 */
typedef enum {
  /** adjust delta value forward */
  WATCHDOG_FORWARD,
  /** adjust delta value backward */
  WATCHDOG_BACKWARD
} Watchdog_Adjust_directions;

/** @brief Watchdog Control Structure
 *
 *  The following record defines the control block used
 *  to manage each watchdog timer.
 */
typedef struct {
  Chain_Node                      Node;
  Watchdog_States                 state;
  Watchdog_Interval               initial;
  Watchdog_Interval               delta_interval;
  Watchdog_Interval               start_time;
  Watchdog_Interval               stop_time;
  Watchdog_Service_routine_entry  routine;
  Objects_Id                      id;
  void                           *user_data;
}   Watchdog_Control;

/** @brief Watchdog Synchronization Level
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_level;

/** @brief Watchdog Synchronization Count
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_count;

/** @brief Ticks Since System Boot
 *
 *  This contains the number of ticks since the system was booted.
 */

SCORE_EXTERN volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/** @brief Per Ticks Watchdog List
 *
 *  This is the watchdog chain which is managed at ticks.
 */
SCORE_EXTERN Chain_Control _Watchdog_Ticks_chain;

/** @brief Per Seconds Watchdog List
 *
 *  This is the watchdog chain which is managed at second boundaries.
 */
SCORE_EXTERN Chain_Control _Watchdog_Seconds_chain;

/** @brief Watchdog Handler Initialization
 *
 *  This routine initializes the watchdog handler.  The watchdog
 *  synchronization flag is initialized and the watchdog chains are
 *  initialized and emptied.
 */
void _Watchdog_Handler_initialization( void );

/** @brief Remove Watchdog from List
 *
 *  This routine removes @a the_watchdog from the watchdog chain on which
 *  it resides and returns the state @a the_watchdog timer was in.
 *  
 *  @param the_watchdog (in) will be removed
 *  @return the state in which @a the_watchdog was in when removed
 */
Watchdog_States _Watchdog_Remove (
  Watchdog_Control *the_watchdog
);

/** @brief Watchdog Adjust
 *
 *  This routine adjusts the @a header watchdog chain in the forward
 *  or backward @a direction for @a units ticks.
 *
 *  @param header (in) is the watchdog chain to adjust
 *  @param direction (in) is the direction to adjust @a header
 *  @param units (in) is the number of units to adjust @a header
 */
void _Watchdog_Adjust (
  Chain_Control              *header,
  Watchdog_Adjust_directions  direction,
  Watchdog_Interval           units
);

/** @brief Watchdog Insert
 *
 *  This routine inserts @a the_watchdog into the @a header watchdog chain
 *  for a time of @a units.  
 *
 *  @param header (in) is @a the_watchdog list to insert @a the_watchdog on
 *  @param the_watchdog (in) is the watchdog to insert
 */
void _Watchdog_Insert (
  Chain_Control         *header,
  Watchdog_Control      *the_watchdog
);

/** @brief Watchdog Tickle
 *
 *  This routine is invoked at appropriate intervals to update
 *  the @a header watchdog chain.
 *
 *  @param header (in) is the watchdog chain to tickle
 */

void _Watchdog_Tickle (
  Chain_Control *header
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/watchdog.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
