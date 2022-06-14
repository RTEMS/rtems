/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * This routine polls to see if a packet has arrived.  If one
 * has it informs the executive. It uses a Classic API Timer
 */

/*
 *  COPYRIGHT (c) 1989-2008, 2016.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/sysstate.h>
#include <rtems/score/watchdogimpl.h>

#include "shm_driver.h"

static void Shm_Poll_Set_timer( Watchdog_Control *the_watchdog )
{
  Per_CPU_Control  *cpu;
  ISR_lock_Context  lock_context;

  cpu = _Watchdog_Get_CPU( the_watchdog );
  _ISR_lock_ISR_disable( &lock_context );
  _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context );
  _Watchdog_Insert(
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
    the_watchdog,
    cpu->Watchdog.ticks + 1
  );
  _Watchdog_Per_CPU_release_critical( cpu, &lock_context );
  _ISR_lock_ISR_enable( &lock_context );
}

static void Shm_Poll_TSR( Watchdog_Control *the_watchdog )
{
  uint32_t tmpfront;

  /*
   *  This should NEVER happen but just in case.
   */
  if (!_System_state_Is_up(_System_state_Get()))
    return;

  tmpfront = Shm_Local_receive_queue->front;
  if ( Shm_Convert(tmpfront) != Shm_Locked_queue_End_of_list ) {
    rtems_multiprocessing_announce();
    Shm_Interrupt_count++;
  }

  Shm_Poll_Set_timer( the_watchdog );
}

static Watchdog_Control Shm_Poll_Watchdog = WATCHDOG_INITIALIZER(
  Shm_Poll_TSR
);

void Shm_install_timer(void)
{
  Shm_Poll_Set_timer( &Shm_Poll_Watchdog );
}
