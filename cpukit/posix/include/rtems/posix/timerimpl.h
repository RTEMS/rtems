/**
 * @file
 *
 * @brief Inlined Routines from the POSIX Timer Manager
 * 
 * This file contains the static inline implementation of the inlined routines
 * from the POSIX Timer Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_TIMERIMPL_H
#define _RTEMS_POSIX_TIMERIMPL_H

#include <rtems/posix/timer.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Timer is free */
#define POSIX_TIMER_STATE_FREE        0x01

/** Created timer but not running */
#define POSIX_TIMER_STATE_CREATE_NEW  0x02

/** Created timer and running */
#define POSIX_TIMER_STATE_CREATE_RUN  0x03

/** Created, ran and stopped timer */
#define POSIX_TIMER_STATE_CREATE_STOP 0x04

/** Indicates that the fire time is relative to the current one */
#define POSIX_TIMER_RELATIVE       0

/*
 * POSIX defines TIMER_ABSTIME but no constant for relative.  So
 * we have one internally but we need to be careful it has a different
 * value.
 */
#if (POSIX_TIMER_RELATIVE == TIMER_ABSTIME)
#error "POSIX_TIMER_RELATIVE == TIMER_ABSTIME"
#endif

/**
 *  @brief POSIX Timer Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Timer_Manager_initialization(void);

/**
 *  @brief POSIX Timer Manager Timer Service Routine Helper
 *
 *  This is the operation that is run when a timer expires.
 */
void _POSIX_Timer_TSR(Objects_Id timer, void *data);

/**
 *  @brief POSIX Timer Watchdog Insertion Helper
 */
bool _POSIX_Timer_Insert_helper(
  Watchdog_Control               *timer,
  Watchdog_Interval               ticks,
  Objects_Id                      id,
  Watchdog_Service_routine_entry  TSR,
  void                           *arg
);

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Timer_Information;

/**
 *  @brief POSIX Timer Allocate
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Timer_Control *_POSIX_Timer_Allocate( void )
{
  return (POSIX_Timer_Control *) _Objects_Allocate( &_POSIX_Timer_Information );
}

/**
 *  @brief POSIX Timer Free
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Timer_Free (
  POSIX_Timer_Control *the_timer
)
{
  _Objects_Free( &_POSIX_Timer_Information, &the_timer->Object );
}

/**
 *  @brief POSIX Timer Get
 *
 *  This function maps timer IDs to timer control blocks.
 *  If ID corresponds to a local timer, then it returns
 *  the timer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */
RTEMS_INLINE_ROUTINE POSIX_Timer_Control *_POSIX_Timer_Get (
  timer_t            id,
  Objects_Locations *location
)
{
  return (POSIX_Timer_Control *)
    _Objects_Get( &_POSIX_Timer_Information, (Objects_Id) id, location );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
