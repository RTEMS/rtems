/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_PRIV_TIMERS Timers
 *
 * @brief Create a Per-Process Timer
 */

/*
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
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
#include <signal.h>

#include <rtems/posix/sigset.h>
#include <rtems/posix/timerimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/seterr.h>
#include <rtems/sysinit.h>

RTEMS_WEAK int _POSIX_Timer_Is_allowed(
  clockid_t clock_id
)
{
  int rc = 0;

  /*
   * Allow timer_create(CLOCK_REALTIME. ...) per POSIX by default
   * on CLOCK_REALTIME or CLOCK_MONOTONIC.
   *
   * But per the FACE Technical Standard, POSIX timers should not be
   * allowed on CLOCK_REALTIME for safety reasons. If the application
   * configures that it wants the FACE behavior, then this method
   * is overridden by <rtems/confdefs/timer.h>.
   */

  if (  clock_id != CLOCK_REALTIME ) {
    if ( clock_id != CLOCK_MONOTONIC ) {
      rc  = EINVAL;
    }
  }

  return rc;
}

int timer_create(
  clockid_t        clock_id,
  struct sigevent *__restrict evp,
  timer_t         *__restrict timerid
)
{
  POSIX_Timer_Control *ptimer;
  int                  rc;

  rc = _POSIX_Timer_Is_allowed( clock_id );
  if ( rc != 0 )
    rtems_set_errno_and_return_minus_one( rc );

  if ( !timerid )
    rtems_set_errno_and_return_minus_one( EINVAL );

 /*
  *  The data of the structure evp are checked in order to verify if they
  *  are coherent.
  */

  if (evp != NULL) {
    /* The structure has data */
    if ( ( evp->sigev_notify != SIGEV_NONE ) &&
         ( evp->sigev_notify != SIGEV_SIGNAL ) ) {
       /* The value of the field sigev_notify is not valid */
       rtems_set_errno_and_return_minus_one( EINVAL );
     }

     if ( !evp->sigev_signo )
       rtems_set_errno_and_return_minus_one( EINVAL );

     if ( !is_valid_signo(evp->sigev_signo) )
       rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /*
   *  Allocate a timer
   */
  ptimer = _POSIX_Timer_Allocate();
  if ( !ptimer ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  /* The data of the created timer are stored to use them later */

  ptimer->state     = POSIX_TIMER_STATE_CREATE_NEW;
  ptimer->thread_id = _Thread_Get_executing()->Object.id;

  if ( evp != NULL ) {
    ptimer->inf.sigev_notify = evp->sigev_notify;
    ptimer->inf.sigev_signo  = evp->sigev_signo;
    ptimer->inf.sigev_value  = evp->sigev_value;
  }

  ptimer->overrun  = 0;
  ptimer->timer_data.it_value.tv_sec     = 0;
  ptimer->timer_data.it_value.tv_nsec    = 0;
  ptimer->timer_data.it_interval.tv_sec  = 0;
  ptimer->timer_data.it_interval.tv_nsec = 0;
  ptimer->clock_type = clock_id;

  _Watchdog_Preinitialize( &ptimer->Timer, _Per_CPU_Get_snapshot() );
  _Watchdog_Initialize( &ptimer->Timer, _POSIX_Timer_TSR );
  _Objects_Open_u32(&_POSIX_Timer_Information, &ptimer->Object, 0);

  *timerid  = ptimer->Object.id;
  _Objects_Allocator_unlock();
  return 0;
}

static void _POSIX_Timer_Manager_initialization( void )
{
  _Objects_Initialize_information( &_POSIX_Timer_Information );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Timer_Manager_initialization,
  RTEMS_SYSINIT_POSIX_TIMER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
