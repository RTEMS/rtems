/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <limits.h> /* _POSIX_PATH_MAX */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/posix/time.h>

/************************************/
/* These includes are now necessary */
/************************************/

#include <unistd.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/clock.h>
#include <rtems/posix/psignal.h>
#include <rtems/score/wkspace.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#include <rtems/posix/timer.h>

/*
 * _POSIX_Timer_Manager_initialization
 *
 *  Description:
 *
 *  Initialize the internal structure in which the data of all
 *  the timers are stored
 */

void _POSIX_Timer_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Timer_Information,  /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_TIMERS,       /* object class */
    Configuration_POSIX_API.maximum_timers,
                                /* maximum objects of this class */
    sizeof( POSIX_Timer_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}
