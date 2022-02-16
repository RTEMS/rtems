/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Deletes a POSIX Interval Timer
 */

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 *
 *  COPYRIGHT (c) 1989-2007.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>
#include <pthread.h>

#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/posix/timerimpl.h>


int timer_delete(
  timer_t timerid
)
{
 /*
  * IDEA: This function must probably stop the timer first and then delete it
  *
  *       It will have to do a call to rtems_timer_cancel and then another
  *       call to rtems_timer_delete.
  *       The call to rtems_timer_delete will be probably unnecessary,
  *       because rtems_timer_delete stops the timer before deleting it.
  */
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context     lock_context;

  _Objects_Allocator_lock();

  ptimer = _POSIX_Timer_Get( timerid, &lock_context );
  if ( ptimer != NULL ) {
    Per_CPU_Control *cpu;

    _Objects_Close( &_POSIX_Timer_Information, &ptimer->Object );
    cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );
    ptimer->state = POSIX_TIMER_STATE_FREE;
    _Watchdog_Remove(
      &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
      &ptimer->Timer
    );
    _POSIX_Timer_Release( cpu, &lock_context );
    _POSIX_Timer_Free( ptimer );
    _Objects_Allocator_unlock();
    return 0;
  }

  _Objects_Allocator_unlock();

  rtems_set_errno_and_return_minus_one( EINVAL );
}
