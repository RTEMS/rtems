/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
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
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/time.h>
#include <rtems/posix/timer.h>


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
  Objects_Locations    location;

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Objects_Close( &_POSIX_Timer_Information, &ptimer->Object );
      ptimer->state = POSIX_TIMER_STATE_FREE;
      (void) _Watchdog_Remove( &ptimer->Timer );
      _POSIX_Timer_Free( ptimer );
      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
