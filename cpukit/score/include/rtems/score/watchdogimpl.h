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
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreWatchdog
 *  @{
 */

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
 * @brief Iterator item to synchronize concurrent insert, remove and tickle
 * operations.
 */
typedef struct {
  /**
   * @brief A node for a Watchdog_Header::Iterators chain.
   */
  Chain_Node Node;

  /**
   * @brief The current delta interval of the new watchdog to insert.
   */
  Watchdog_Interval delta_interval;

  /**
   * @brief The current watchdog of the chain on the way to insert the new
   * watchdog.
   */
  Chain_Node *current;
} Watchdog_Iterator;

/**
 * @brief Watchdog header.
 */
typedef struct {
  /**
   * @brief ISR lock to protect this watchdog chain.
   */
  ISR_LOCK_MEMBER( Lock )

  /**
   * @brief The chain of active or transient watchdogs.
   */
  Chain_Control Watchdogs;

  /**
   * @brief Currently active iterators.
   *
   * The iterators are registered in _Watchdog_Insert() and updated in case the
   * watchdog chain changes.
   */
  Chain_Control Iterators;
} Watchdog_Header;

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

RTEMS_INLINE_ROUTINE void _Watchdog_Acquire(
  Watchdog_Header  *header,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire( &header->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Release(
  Watchdog_Header  *header,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &header->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Watchdog_Flash(
  Watchdog_Header  *header,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Flash( &header->Lock, lock_context );
}

/**
 *  @brief Initialize the watchdog handler.
 *
 *  This routine initializes the watchdog handler.  The watchdog
 *  synchronization flag is initialized and the watchdog chains are
 *  initialized and emptied.
 */
void _Watchdog_Handler_initialization( void );

/**
 *  @brief Triggers a watchdog tick.
 *
 *  This routine executes TOD, watchdog and scheduler ticks.
 */
void _Watchdog_Tick( void );

/**
 *  @brief Removes @a the_watchdog from the watchdog chain.
 *
 *  This routine removes @a the_watchdog from the watchdog chain on which
 *  it resides and returns the state @a the_watchdog timer was in.
 *
 *  @param[in] header The watchdog chain.
 *  @param[in] the_watchdog will be removed
 *  @retval the state in which @a the_watchdog was in when removed
 */
Watchdog_States _Watchdog_Remove (
  Watchdog_Header  *header,
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
 * @brief Adjusts the watchdogs in backward direction in a locked context.
 *
 * The caller must be the owner of the watchdog lock and will be the owner
 * after the call.
 *
 * @param[in] header The watchdog header.
 * @param[in] units The units of ticks to adjust.
 *
 * @see _Watchdog_Adjust_forward().
 */
void _Watchdog_Adjust_backward_locked(
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
 * @brief Adjusts the watchdogs in forward direction in a locked context.
 *
 * The caller must be the owner of the watchdog lock and will be the owner
 * after the call.  This function may release and acquire the watchdog lock
 * internally.
 *
 * @param[in] header The watchdog header.
 * @param[in] units The units of ticks to adjust.
 * @param[in] lock_context The lock context.
 *
 * @see _Watchdog_Adjust_forward().
 */
void _Watchdog_Adjust_forward_locked(
  Watchdog_Header   *header,
  Watchdog_Interval  units,
  ISR_lock_Context  *lock_context
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
 * @brief Inserts the watchdog in a locked context.
 *
 * The caller must be the owner of the watchdog lock and will be the owner
 * after the call.  This function may release and acquire the watchdog lock
 * internally.
 *
 * @param[in] header The watchdog header.
 * @param[in] the_watchdog The watchdog.
 * @param[in] lock_context The lock context.
 *
 * @see _Watchdog_Insert().
 */
void _Watchdog_Insert_locked(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog,
  ISR_lock_Context *lock_context
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
 * @brief Pre-initializes a watchdog.
 *
 * This routine must be called before a watchdog is used in any way.  The
 * exception are statically initialized watchdogs via WATCHDOG_INITIALIZER().
 *
 * @param[in] the_watchdog The uninitialized watchdog.
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Preinitialize(
  Watchdog_Control *the_watchdog
)
{
  the_watchdog->state = WATCHDOG_INACTIVE;
#if defined(RTEMS_DEBUG)
  the_watchdog->routine = NULL;
  the_watchdog->id = 0;
  the_watchdog->user_data = NULL;
#endif
}

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
  _Assert( the_watchdog->state == WATCHDOG_INACTIVE );
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

RTEMS_INLINE_ROUTINE Watchdog_States _Watchdog_Remove_ticks(
  Watchdog_Control *the_watchdog
)
{
  return _Watchdog_Remove( &_Watchdog_Ticks_header, the_watchdog );
}

RTEMS_INLINE_ROUTINE Watchdog_States _Watchdog_Remove_seconds(
  Watchdog_Control *the_watchdog
)
{
  return _Watchdog_Remove( &_Watchdog_Seconds_header, the_watchdog );
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

  _Watchdog_Remove_ticks( the_watchdog );

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
  _ISR_lock_Initialize( &header->Lock, "Watchdog" );
  _Chain_Initialize_empty( &header->Watchdogs );
  _Chain_Initialize_empty( &header->Iterators );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
