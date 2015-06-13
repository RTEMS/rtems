/**
 *  @file  rtems/score/watchdog.h
 *
 *  @brief Constants and Structures Associated with Watchdog Timers
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WATCHDOG_H
#define _RTEMS_SCORE_WATCHDOG_H

#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 *  @brief Type is used to specify the length of intervals.
 *
 *  This type is used to specify the length of intervals.
 */
typedef uint32_t   Watchdog_Interval;

/**
 *  @brief Return type from a Watchdog Service Routine.
 *
 *  This type defines the return type from a Watchdog Service Routine.
 */
typedef void Watchdog_Service_routine;

/**
 *  @brief Pointer to a watchdog service routine.
 *
 *  This type define a pointer to a watchdog service routine.
 */
typedef Watchdog_Service_routine ( *Watchdog_Service_routine_entry )(
                 Objects_Id,
                 void *
             );

/**
 *  @brief The constant for indefinite wait.
 *
 *  This is the constant for indefinite wait.  It is actually an
 *  illegal interval.
 */
#define WATCHDOG_NO_TIMEOUT  0

/**
 *  @brief Set of the states which a watchdog timer may be at any given time.
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
  WATCHDOG_ACTIVE
} Watchdog_States;

/**
 *  @brief The control block used to manage each watchdog timer.
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
 * @brief The watchdog ticks counter.
 *
 * With a 1ms watchdog tick, this counter overflows after 50 days since boot.
 */
extern volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
