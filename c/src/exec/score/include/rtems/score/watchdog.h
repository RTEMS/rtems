/*  watchdog.h
 *
 *  This include file contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *   used to initialize and manipulate watchdog timers.
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
 *  The following enumerated type details the modes in which the
 *  Watchdog_Insert routine may operate.  The watchdog may be
 *  activated automatically at insert time or later, explicitly
 *  by the caller.
 */

typedef enum {
  WATCHDOG_ACTIVATE_NOW, /* activate watchdog as part of insertion */
  WATCHDOG_NO_ACTIVATE   /* watchdog will be explicitly activated */
} Watchdog_Insert_modes;

/*
 *  The following enumerated type lists the states in which a
 *  watchdog timer may be at any given time.
 */

typedef enum {
  WATCHDOG_INACTIVE,     /* off all chains */
  WATCHDOG_ACTIVE,       /* on chain, allowed to fire */
  WATCHDOG_REINSERT,     /* on chain, reset without firing if expires */
  WATCHDOG_REMOVE_IT     /* on chain, remove without firing if expires */
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
  Chain_Node        Node;
  Watchdog_States   state;
  Watchdog_Interval initial;
  Watchdog_Interval delta_interval;
  Watchdog_Service_routine_entry  routine;
  Objects_Id        id;
  void             *user_data;
}   Watchdog_Control;

/*
 *  The following are used for synchronization purposes
 *  during an insert on a watchdog delta chain.
 */

EXTERN volatile unsigned32  _Watchdog_Sync_level;
EXTERN volatile unsigned32  _Watchdog_Sync_count;

/*
 *  The following defines the watchdog chains which are managed
 *  on ticks and second boundaries.
 */

EXTERN Chain_Control _Watchdog_Ticks_chain;
EXTERN Chain_Control _Watchdog_Seconds_chain;

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
 *
 *  _Watchdog_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the specified watchdog.  The watchdog is
 *  made inactive, the watchdog id and handler routine are set to the
 *  specified values.
 */

STATIC INLINE void _Watchdog_Initialize(
   Watchdog_Control *the_watchdog,
   Watchdog_Service_routine_entry  routine,
   Objects_Id        id,
   void             *user_data
);

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
 *
 *  _Watchdog_Is_active
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the watchdog timer is in the ACTIVE
 *  state, and FALSE otherwise.
 */

STATIC INLINE boolean _Watchdog_Is_active(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_Activate
 *
 *  DESCRIPTION:
 *
 *  This routine activates THE_WATCHDOG timer which is already
 *  on a watchdog chain.
 */

STATIC INLINE void _Watchdog_Activate(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_Deactivate
 *
 *  DESCRIPTION:
 *
 *  This routine deactivates THE_WATCHDOG timer which will remain
 *  on a watchdog chain.
 */

STATIC INLINE void _Watchdog_Deactivate(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_Tickle_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked at each clock tick to update the ticks
 *  watchdog chain.
 */

STATIC INLINE void _Watchdog_Tickle_ticks( void );

/*
 *
 *  _Watchdog_Tickle_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked at each clock tick to update the seconds
 *  watchdog chain.
 */

STATIC INLINE void _Watchdog_Tickle_seconds( void );

/*
 *
 *  _Watchdog_Insert_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine inserts THE_WATCHDOG into the ticks watchdog chain
 *  for a time of UNITS ticks.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 */

STATIC INLINE void _Watchdog_Insert_ticks(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units,
  Watchdog_Insert_modes  insert_mode
);

/*
 *
 *  _Watchdog_Insert_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine inserts THE_WATCHDOG into the seconds watchdog chain
 *  for a time of UNITS seconds.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 */

STATIC INLINE void _Watchdog_Insert_seconds(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units,
  Watchdog_Insert_modes  insert_mode
);

/*
 *
 *  _Watchdog_Adjust_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine adjusts the seconds watchdog chain in the forward
 *  or backward DIRECTION for UNITS seconds.  This is invoked when the
 *  current time of day is changed.
 */

STATIC INLINE void _Watchdog_Adjust_seconds(
  Watchdog_Adjust_directions direction,
  Watchdog_Interval          units
);

/*
 *
 *  _Watchdog_Adjust_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine adjusts the ticks watchdog chain in the forward
 *  or backward DIRECTION for UNITS ticks.
 */

STATIC INLINE void _Watchdog_Adjust_ticks(
  Watchdog_Adjust_directions direction,
  Watchdog_Interval          units
);

/*
 *
 *  _Watchdog_Reset
 *
 *  DESCRIPTION:
 *
 *  This routine resets THE_WATCHDOG timer to its state at INSERT
 *  time.  This routine is valid only on interval watchdog timers
 *  and is used to make an interval watchdog timer fire "every" so
 *  many ticks.
 */

STATIC INLINE void _Watchdog_Reset(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_Next
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the watchdog timer following
 *  THE_WATCHDOG on the watchdog chain.
 */

STATIC INLINE Watchdog_Control *_Watchdog_Next(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_Previous
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the watchdog timer preceding
 *  THE_WATCHDOG on the watchdog chain.
 */

STATIC INLINE Watchdog_Control *_Watchdog_Previous(
  Watchdog_Control *the_watchdog
);

/*
 *
 *  _Watchdog_First
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the first watchdog timer
 *  on the watchdog chain HEADER.
 */

STATIC INLINE Watchdog_Control *_Watchdog_First(
  Chain_Control *header
);

/*
 *
 *  _Watchdog_Last
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the last watchdog timer
 *  on the watchdog chain HEADER.
 */
STATIC INLINE Watchdog_Control *_Watchdog_Last(
  Chain_Control *header
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
  Watchdog_Control      *the_watchdog,
  Watchdog_Insert_modes  insert_mode
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

#include <rtems/score/watchdog.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
