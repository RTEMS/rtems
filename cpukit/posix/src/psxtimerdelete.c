/**
 * @file
 *
 * @brief Deletes a POSIX Interval Timer
 * @ingroup POSIXAPI
 */

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
