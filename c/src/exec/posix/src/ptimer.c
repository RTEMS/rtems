/*
 *  ptimer.c,v 1.1 1996/06/03 16:29:58 joel Exp
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
 
#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/posix/time.h>

/************************************/
/* These includes are now necessary */
/************************************/

#include <sys/features.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/clock.h>
#include <rtems/posix/psignal.h>
#include <rtems/score/wkspace.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

/*****************************/
/* End of necessary includes */
/*****************************/

#include <rtems/posix/timer.h>

/* ***************************************************************************
 * TIMER_INITIALIZE_S
 *
 *  Description: Initialize the data of a timer
 * ***************************************************************************/

void TIMER_INITIALIZE_S ( int timer_pos )
{

   /*
    * Indicates that the position in the table is free
    */

    timer_struct[timer_pos].state = STATE_FREE_C;

   /*
    * The initial data of timing are set with null value
    */

    timer_struct[timer_pos].timer_data.it_value.tv_sec     = 0;
    timer_struct[timer_pos].timer_data.it_value.tv_nsec    = 0;
    timer_struct[timer_pos].timer_data.it_interval.tv_sec  = 0;
    timer_struct[timer_pos].timer_data.it_interval.tv_nsec = 0;

   /*
    * The count of expirations is 0
    */

    timer_struct[timer_pos].overrun = 0;

}

/* ***************************************************************************
 * _POSIX_Timer_Manager_initialization
 *
 *  Description: Initialize the internal structure in which the data of all 
 *               the timers are stored
 * ***************************************************************************/

int timer_max;
POSIX_Timer_Control *timer_struct;


void _POSIX_Timer_Manager_initialization ( int max_timers )
{
   int index;

   timer_struct = _Workspace_Allocate_or_fatal_error(
      max_timers * sizeof(POSIX_Timer_Control) );

   /* 
    *  Initialize all the timers
    */

   timer_max = max_timers;

   for (index=0; index<max_timers; index++)
     TIMER_INITIALIZE_S( index );

   timer_max = max_timers;
}

