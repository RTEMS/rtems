/*
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/time.h>
#include <rtems/posix/timer.h>

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
)
{
  POSIX_Timer_Control *ptimer;

  if ( clock_id != CLOCK_REALTIME )
    rtems_set_errno_and_return_minus_one( EINVAL );

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

  _Thread_Disable_dispatch();         /* to prevent deletion */

  /*
   *  Allocate a timer
   */
  ptimer = _POSIX_Timer_Allocate();
  if ( !ptimer ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  /* The data of the created timer are stored to use them later */

  ptimer->state     = POSIX_TIMER_STATE_CREATE_NEW;
  ptimer->thread_id = _Thread_Executing->Object.id;

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

  _Watchdog_Initialize( &ptimer->Timer, NULL, 0, NULL );
  _Objects_Open_u32(&_POSIX_Timer_Information, &ptimer->Object, 0);

  *timerid  = ptimer->Object.id;
  _Thread_Enable_dispatch();
  return 0;
}
