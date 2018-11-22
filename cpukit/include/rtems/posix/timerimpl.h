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
#include <rtems/score/watchdogimpl.h>

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

void _POSIX_Timer_TSR( Watchdog_Control *the_watchdog );

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
  ISR_lock_Context  *lock_context
)
{
  return (POSIX_Timer_Control *) _Objects_Get(
    (Objects_Id) id,
    lock_context,
    &_POSIX_Timer_Information
  );
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *_POSIX_Timer_Acquire_critical(
  POSIX_Timer_Control *ptimer,
  ISR_lock_Context    *lock_context
)
{
  Per_CPU_Control *cpu;

  cpu = _Watchdog_Get_CPU( &ptimer->Timer );
  _Watchdog_Per_CPU_acquire_critical( cpu, lock_context );

  return cpu;
}

RTEMS_INLINE_ROUTINE void _POSIX_Timer_Release(
  Per_CPU_Control  *cpu,
  ISR_lock_Context *lock_context
)
{
  _Watchdog_Per_CPU_release_critical( cpu, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
