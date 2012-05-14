/**
 *  @file  rtems/score/watchdog.h
 *
 *  This include file contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *  used to initialize and manipulate watchdog timers.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WATCHDOG_H
#define _RTEMS_SCORE_WATCHDOG_H

/**
 *  @defgroup ScoreWatchdog Watchdog Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the scheduling of
 *  watchdog functions to be called at specific times in the future.
 *
 *  @note This handler does not have anything to do with hardware watchdog
 *        timers.
 */
/**@{*/

#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Maximum Interval Length
 *
 *  The following type defines the control block used to manage
 *  intervals.
 */
#define WATCHDOG_MAXIMUM_INTERVAL ((Watchdog_Interval) 0xffffffff)

/**
 *  @brief Watchdog Interval Type
 *
 *  This type is used to specify the length of intervals.
 */
typedef uint32_t   Watchdog_Interval;

/**
 *  @brief Watchdog Nanoseconds Since Last Tick Extension
 *
 *  This type defines a pointer to the BSP plugin to obtain the number
 *  of nanoseconds since the last clock tick.
 */
typedef uint32_t (*Watchdog_Nanoseconds_since_last_tick_routine)(void);

/**
 *  @brief Watchdog Service Routine Return Type
 *
 *  This type defines the return type from a Watchdog Service Routine.
 */
typedef void Watchdog_Service_routine;

/**
 *  @brief Watchdog Service Routine Pointer Type
 *
 *  This type define a pointer to a watchdog service routine.
 */
typedef Watchdog_Service_routine ( *Watchdog_Service_routine_entry )(
                 Objects_Id,
                 void *
             );

/**
 *  @brief No timeout constant
 *
 *  This is the constant for indefinite wait.  It is actually an
 *  illegal interval.
 */
#define WATCHDOG_NO_TIMEOUT  0

/**
 *  @brief Watchdog States Type
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

/**
 *  @brief Watchdog Adjustment Directions Type
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

/**
 *  @brief Watchdog Control Structure
 *
 *  The following record defines the control block used
 *  to manage each watchdog timer.
 */
typedef struct {
  /** This field is a Chain Node structure and allows this to be placed on
   *  chains for set management.
   */
  Chain_Node                      Node;
  /** This field is the state of the watchdog. */
  Watchdog_States                 state;
  /** This field is the initially requested interval. */
  Watchdog_Interval               initial;
  /** This field is the remaining portion of the interval. */
  Watchdog_Interval               delta_interval;
  /** This field is the number of system clock ticks when this was scheduled. */
  Watchdog_Interval               start_time;
  /** This field is the number of system clock ticks when this was suspended. */
  Watchdog_Interval               stop_time;
  /** This field is the function to invoke. */
  Watchdog_Service_routine_entry  routine;
  /** This field is the Id to pass as an argument to the routine. */
  Objects_Id                      id;
  /** This field is an untyped pointer to user data that is passed to the
   *  watchdog handler routine.
   */
  void                           *user_data;
}   Watchdog_Control;

/**
 *  @brief Watchdog Synchronization Level
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_level;

/**
 *  @brief Watchdog Synchronization Count
 *
 *  This used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */
SCORE_EXTERN volatile uint32_t    _Watchdog_Sync_count;

/**
 *  @brief Ticks Since System Boot
 *
 *  This contains the number of ticks since the system was booted.
 */

SCORE_EXTERN volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/**
 *  @brief Watchdog Nanoseconds Since Last Tick Handler
 *
 *  This is a pointer to the optional BSP plugin to obtain the number
 *  of nanoseconds since the last clock tick.
 */
extern Watchdog_Nanoseconds_since_last_tick_routine
  _Watchdog_Nanoseconds_since_tick_handler;

/**
 *  @brief Per Ticks Watchdog List
 *
 *  This is the watchdog chain which is managed at ticks.
 */
SCORE_EXTERN Chain_Control _Watchdog_Ticks_chain;

/**
 *  @brief Per Seconds Watchdog List
 *
 *  This is the watchdog chain which is managed at second boundaries.
 */
SCORE_EXTERN Chain_Control _Watchdog_Seconds_chain;

/**
 *  @brief Watchdog Handler Initialization
 *
 *  This routine initializes the watchdog handler.  The watchdog
 *  synchronization flag is initialized and the watchdog chains are
 *  initialized and emptied.
 */
void _Watchdog_Handler_initialization( void );

/**
 *  @brief Remove Watchdog from List
 *
 *  This routine removes @a the_watchdog from the watchdog chain on which
 *  it resides and returns the state @a the_watchdog timer was in.
 *
 *  @param[in] the_watchdog will be removed
 *  @return the state in which @a the_watchdog was in when removed
 */
Watchdog_States _Watchdog_Remove (
  Watchdog_Control *the_watchdog
);

/**
 *  @brief Watchdog Adjust
 *
 *  This routine adjusts the @a header watchdog chain in the forward
 *  or backward @a direction for @a units ticks.
 *
 *  @param[in] header is the watchdog chain to adjust
 *  @param[in] direction is the direction to adjust @a header
 *  @param[in] units is the number of units to adjust @a header
 */
void _Watchdog_Adjust (
  Chain_Control              *header,
  Watchdog_Adjust_directions  direction,
  Watchdog_Interval           units
);

/**
 *  @brief Watchdog Adjust to Chain
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
  Chain_Control               *header,
  Watchdog_Interval            units_arg,
  Chain_Control               *to_fire

);

/**
 *  @brief Watchdog Insert
 *
 *  This routine inserts @a the_watchdog into the @a header watchdog chain
 *  for a time of @a units.
 *
 *  @param[in] header is @a the_watchdog list to insert @a the_watchdog on
 *  @param[in] the_watchdog is the watchdog to insert
 */
void _Watchdog_Insert (
  Chain_Control         *header,
  Watchdog_Control      *the_watchdog
);

/**
 *  @brief Watchdog Tickle
 *
 *  This routine is invoked at appropriate intervals to update
 *  the @a header watchdog chain.
 *
 *  @param[in] header is the watchdog chain to tickle
 */
void _Watchdog_Tickle (
  Chain_Control *header
);

/**
 *  @brief Report Information on a Single Watchdog Instance
 *
 *  This method prints a one line report on the watchdog instance
 *  provided.  The @a name may be used to identify the watchdog and
 *  a space will be printed after @a name if it is not NULL.
 *
 *  @param[in] name is a string to prefix the line with.  If NULL,
 *             nothing is printed.
 *  @param[in] watch is the watchdog instance to be printed.
 *
 *  @note This is a debug routine.  It uses printk() and prudence should
 *        exercised when using it.
 */
void _Watchdog_Report(
  const char        *name,
  Watchdog_Control  *watch
);

/**
 *  @brief Report Information on a Watchdog Chain
 *
 *  This method prints report on the watchdog chain provided.
 *  The @a name may be used to identify the watchdog chain and
 *  a space will be printed after @a name if it is not NULL.
 *
 *  @param[in] name is a string to prefix the line with.  If NULL,
 *             nothing is printed.
 *  @param[in] header is the watchdog chain to be printed.
 *
 *  @note This is a debug routine.  It uses printk() and prudence should
 *        exercised when using it.  It also disables interrupts so the
 *        chain can be traversed in a single atomic pass.
 */
void _Watchdog_Report_chain(
  const char        *name,
  Chain_Control     *header
);

/**
 * @brief Default nanoseconds since last tick handler.
 *
 * @retval 0 Always.
 */
uint32_t _Watchdog_Nanoseconds_since_tick_default_handler( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/watchdog.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
