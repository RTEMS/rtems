/**
 * @file
 *
 * @brief Process Timer
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <limits.h> /* _POSIX_PATH_MAX */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>


/************************************/
/* These includes are now necessary */
/************************************/

#include <unistd.h>
#include <rtems/sysinit.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/wkspace.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#include <rtems/posix/timerimpl.h>

Objects_Information _POSIX_Timer_Information;

/*
 * _POSIX_Timer_Manager_initialization
 *
 *  Description:
 *
 *  Initialize the internal structure in which the data of all
 *  the timers are stored
 */

static void _POSIX_Timer_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Timer_Information,  /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_TIMERS,       /* object class */
    _Configuration_POSIX_Maximum_timers,
    sizeof( POSIX_Timer_Control ),
                                /* size of this object's control block */
    false,                      /* true if names for this object are strings */
    0,                          /* maximum length of each object's name */
    NULL                        /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Timer_Manager_initialization,
  RTEMS_SYSINIT_POSIX_TIMER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
