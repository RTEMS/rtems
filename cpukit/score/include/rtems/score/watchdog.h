/*  watchdog.h
 *
 *  This include file contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *   used to initialize and manipulate watchdog timers.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __WATCHDOG_h
#define __WATCHDOG_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>

/*
 *  The following type defines the control block used to manage
 *  intervals.
 */

#define WATCHDOG_MAXIMUM_INTERVAL ((Watchdog_Interval) 0xffffffff)

typedef unsigned32 Watchdog_Interval;

/*
 *  The following types define a pointer to a watchdog service routine.
 */

typedef void Watchdog_Service_routine;

typedef Watchdog_Service_routine ( *Watchdog_Service_routine_entry )(
                 Objects_Id,
                 void *
             );

/*
 *  Constant for indefinite wait.  (actually an illegal interval)
 */

#define WATCHDOG_NO_TIMEOUT  0

/*
 *  The following enumerated type lists the states in which a
 *  watchdog timer may be at any given time.
 */

typedef enum {
  WATCHDOG_INACTIVE,       /* off all chains */
  WATCHDOG_BEING_INSERTED, /* off all chains, searching for insertion point */
  WATCHDOG_ACTIVE,         /* on chain, allowed to fire */
  WATCHDOG_REMOVE_IT       /* on chain, remove without firing if expires */
} Watchdog_States;

/*
 *  The following enumerated type details the manner in which
 *  a watchdog chain may be adjusted by the Watchdog_Adjust
 *  routine.  The direction indicates a movement FORWARD
 *  or BACKWARD in time.
 */

typedef enum {
  WATCHDOG_FORWARD,      /* adjust delta value forward */
  WATCHDOG_BACKWARD      /* adjust delta value backward */
} Watchdog_Adjust_directions;

/*
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

/*
 *  The following are used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */

SCORE_EXTERN volatile unsigned32  _Watchdog_Sync_level;
SCORE_EXTERN volatile unsigned32  _Watchdog_Sync_count;

/*
 *  The following contains the number of ticks since the
 *  system was booted.
 */

SCORE_EXTERN volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/*
 *  The following defines the watchdog chains which are managed
 *  on ticks and second boundaries.
 */

SCORE_EXTERN Chain_Control _Watchdog_Ticks_chain;
SCORE_EXTERN Chain_Control _Watchdog_Seconds_chain;

/*
 *  _Watchdog_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the watchdog handler.  The watchdog
 *  synchronization flag is initialized and the watchdog chains are
 *  initialized and emptied.
 */

void _Watchdog_Handler_initialization( void );

/*
 *  _Watchdog_Remove
 *
 *  DESCRIPTION:
 *
 *  This routine removes THE_WATCHDOG from the watchdog chain on which
 *  it resides and returns the state THE_WATCHDOG timer was in.
 */

Watchdog_States _Watchdog_Remove (
  Watchdog_Control *the_watchdog
);

/*
 *  _Watchdog_Adjust
 *
 *  DESCRIPTION:
 *
 *  This routine adjusts the HEADER watchdog chain in the forward
 *  or backward DIRECTION for UNITS ticks.
 */

void _Watchdog_Adjust (
  Chain_Control              *header,
  Watchdog_Adjust_directions  direction,
  Watchdog_Interval           units
);

/*
 *  _Watchdog_Insert
 *
 *  DESCRIPTION:
 *
 *  This routine inserts THE_WATCHDOG into the HEADER watchdog chain
 *  for a time of UNITS.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 *
 */

void _Watchdog_Insert (
  Chain_Control         *header,
  Watchdog_Control      *the_watchdog
);

/*
 *  _Watchdog_Tickle
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked at appropriate intervals to update
 *  the HEADER watchdog chain.
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

#endif
/* end of include file */
