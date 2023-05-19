/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTimer
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicTimer.
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_RTEMS_TIMER_INL
#define _RTEMS_RTEMS_TIMER_INL

#include <rtems/rtems/timerdata.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicTimer Timer Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Timer Manager implementation.
 *
 * @{
 */

typedef struct Timer_server_Control {
  ISR_LOCK_MEMBER( Lock )

  Chain_Control Pending;

  Objects_Id server_id;
} Timer_server_Control;

/**
 * @brief Pointer to default timer server control block.
 *
 * This value is @c NULL when the default timer server is not initialized.
 */
extern Timer_server_Control *volatile _Timer_server;

/**
 *  @brief Timer_Allocate
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */
static inline Timer_Control *_Timer_Allocate( void )
{
  return (Timer_Control *) _Objects_Allocate( &_Timer_Information );
}

/**
 *  @brief Timer_Free
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */
static inline void _Timer_Free (
  Timer_Control *the_timer
)
{
  _Objects_Free( &_Timer_Information, &the_timer->Object );
}

static inline Timer_Control *_Timer_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
)
{
  return (Timer_Control *) _Objects_Get(
    id,
    lock_context,
    &_Timer_Information
  );
}

static inline Per_CPU_Control *_Timer_Acquire_critical(
  Timer_Control    *the_timer,
  ISR_lock_Context *lock_context
)
{
  Per_CPU_Control *cpu;

  cpu = _Watchdog_Get_CPU( &the_timer->Ticker );
  _Watchdog_Per_CPU_acquire_critical( cpu, lock_context );

  return cpu;
}

static inline void _Timer_Release(
  Per_CPU_Control  *cpu,
  ISR_lock_Context *lock_context
)
{
  _Watchdog_Per_CPU_release_critical( cpu, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

static inline bool _Timer_Is_interval_class(
  Timer_Classes the_class
)
{
  int mask;

  mask = TIMER_CLASS_BIT_NOT_DORMANT | TIMER_CLASS_BIT_TIME_OF_DAY;
  return ( the_class & mask ) == TIMER_CLASS_BIT_NOT_DORMANT;
}

static inline bool _Timer_Is_on_task_class(
  Timer_Classes the_class
)
{
  int mask;

  mask = TIMER_CLASS_BIT_NOT_DORMANT | TIMER_CLASS_BIT_ON_TASK;
  return ( the_class & mask ) == mask;
}

static inline Per_CPU_Watchdog_index _Timer_Watchdog_header_index(
  Timer_Classes the_class
)
{
  return (Per_CPU_Watchdog_index) ( the_class & TIMER_CLASS_BIT_TIME_OF_DAY );
}

static inline Watchdog_Interval _Timer_Get_CPU_ticks(
  const Per_CPU_Control *cpu
)
{
  return (Watchdog_Interval) cpu->Watchdog.ticks;
}

rtems_status_code _Timer_Fire(
  rtems_id                           id,
  rtems_interval                     interval,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

rtems_status_code _Timer_Fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

rtems_status_code _Timer_Fire_when(
  rtems_id                           id,
  const rtems_time_of_day           *wall_time,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

void _Timer_Cancel( Per_CPU_Control *cpu, Timer_Control *the_timer );

void _Timer_Routine_adaptor( Watchdog_Control *the_watchdog );

void _Timer_server_Routine_adaptor( Watchdog_Control *the_watchdog );

static inline void _Timer_server_Acquire_critical(
  Timer_server_Control *timer_server,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Acquire( &timer_server->Lock, lock_context );
}

static inline void _Timer_server_Release_critical(
  Timer_server_Control *timer_server,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Release( &timer_server->Lock, lock_context );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
