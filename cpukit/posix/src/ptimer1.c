/*
 *  ptimer.c,v 1.1 1996/06/03 16:29:58 joel Exp
 */
 
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

#include <rtems/seterr.h>
#include <rtems/posix/timer.h>

/*****************************/
/* End of necessary includes */
/*****************************/

/* ************ 
 * Constants
 * ************/ 

/*
#define DEBUG_MESSAGES
 */

/*
 * Data for the signals 
 */

/***********************************
 * Definition of Internal Functions
 ***********************************/

/* ***************************************************************************
 * TIMER_INITIALIZE_S
 *
 *  Description: Initialize the data of a timer 
 * ***************************************************************************/

extern void TIMER_INITIALIZE_S ( int timer_pos );

/* ***************************************************************************
 * _POSIX_Timer_Manager_initialization
 *
 *  Description: Initialize the internal structure in which the data of all 
 *               the timers are stored
 * ***************************************************************************/

/* split to reduce minimum size */

/* ***************************************************************************
 * FIRST_FREE_POSITION_F 
 *
 *  Description: Returns the first free position in the table of timers.
 *               If there is not a free position, it returns NO_MORE_TIMERS_C 
 * ***************************************************************************/

int FIRST_FREE_POSITION_F ()
{
   int index;

   for (index=0; index<timer_max; index++) {
      if ( timer_struct[index].state == STATE_FREE_C ) {
         return index;
      }
   }
   
   /* The function reaches this point only if all the position are occupied */

   return NO_MORE_TIMERS_C;
}

/* ***************************************************************************
 * TIMER_POSITION_F 
 *
 *  Description: Returns the position in the table of timers in which the
 *               data of the timer are stored.
 *               If the timer identifier does not exist, it returns
 *               BAD_TIMER_C
 * ***************************************************************************/

int TIMER_POSITION_F ( timer_t timer_id )
{
  int index;

  for (index=0; index<timer_max; index++ ) {

     /* Looks for the position of the timer. The timer must exist and the
      * position can not be free */
     if ( ( timer_struct[index].timer_id == timer_id ) &&
          ( timer_struct[index].state != STATE_FREE_C ) ) {
        return index;
     }
  }

  /* If the function reaches this point is because the timer identifier
   * is not correct */

   return BAD_TIMER_C;

}

/* ***************************************************************************
 * COPY_ITIMERSPEC_S 
 *
 *  Description: Does a copy of a variable of type struct itimerspec  
 * ***************************************************************************/

void COPY_ITIMERSPEC_S ( const struct itimerspec *source, 
                         struct itimerspec *target )
{

   target->it_value.tv_sec     = source->it_value.tv_sec;
   target->it_value.tv_nsec    = source->it_value.tv_nsec;
   target->it_interval.tv_sec  = source->it_interval.tv_sec;
   target->it_interval.tv_nsec = source->it_interval.tv_nsec;

}

/* ***************************************************************************
 * ITIMERSPEC_TO_RTEMS_TIME_OF_DAY_S 
 *
 *  Description: This function converts the data of a structure itimerspec 
 *               into structure rtems_time_of_day 
 * ***************************************************************************/

void ITIMERSPEC_TO_RTEMS_TIME_OF_DAY_S 
   ( const struct itimerspec *itimer, rtems_time_of_day *rtems_time )
{
   unsigned long int seconds;

   /* The leap years and the months with 28, 29 or 31 days have not been
    * considerated. It will be made in the future */ 

   seconds            = itimer->it_value.tv_sec;

   rtems_time->year   = seconds / SECONDS_PER_YEAR_C;
   seconds            = seconds % SECONDS_PER_YEAR_C;

   rtems_time->month  = seconds / SECONDS_PER_MONTH_C;
   seconds            = seconds % SECONDS_PER_MONTH_C;

   rtems_time->day    = seconds / SECONDS_PER_DAY_C;
   seconds            = seconds % SECONDS_PER_DAY_C;

   rtems_time->hour   = seconds / SECONDS_PER_HOUR_C;
   seconds            = seconds % SECONDS_PER_HOUR_C;

   rtems_time->minute = seconds / SECONDS_PER_MINUTE_C;
   seconds            = seconds % SECONDS_PER_MINUTE_C;

   rtems_time->second = seconds;

   rtems_time->ticks  = ( itimer->it_value.tv_nsec * SEC_TO_TICKS_C ) /
                        NSEC_PER_SEC_C;

}


/* ***************************************************************************
 * FIRE_TIMER_S 
 *
 *  Description: This is the operation that is ran when a timer expires
 * ***************************************************************************/


rtems_timer_service_routine FIRE_TIMER_S (rtems_id timer, void *data) 
{
  int               timer_pos;  /* Position in the table of the timer that    
			         *  has expirated 			     */
  rtems_status_code return_v;   /* Return value of rtems_timer_fire_after    */
  int               sig_number; /* Number of the signal to send              */

 
  /* The position of the table of timers that contains the data of the 
   * expired timer will be stored in "timer_pos". In theory a timer can not
   * expire if it has not been created or has been deleted */ 

  timer_pos = TIMER_POSITION_F(timer);

  /* Increases the number of expiration of the timer in one unit. */
  timer_struct[timer_pos].overrun = timer_struct[timer_pos].overrun + 1;


  if ( ( timer_struct[timer_pos].timer_data.it_interval.tv_sec  != 0 ) ||
       ( timer_struct[timer_pos].timer_data.it_interval.tv_nsec != 0 ) ) {

     /* The timer must be reprogrammed */

     return_v = rtems_timer_fire_after ( timer, 
                                        timer_struct[timer_pos].ticks, 
                                        FIRE_TIMER_S, 
                                        NULL );

     /* Stores the time when the timer was started again */ 

     timer_struct[timer_pos].time = _TOD_Current;
     
     /* The state has not to be actualized, because nothing modifies it */

     timer_struct[timer_pos].state = STATE_CREATE_RUN_C;

  } else {
     /* Indicates that the timer is stopped */
 
     timer_struct[timer_pos].state = STATE_CREATE_STOP_C;

  }

  /* 
   * The sending of the signal to the process running the handling function
   * specified for that signal is simulated
   */

  sig_number = timer_struct[timer_pos].inf.sigev_signo;

  if( pthread_kill ( timer_struct[timer_pos].thread_id ,
                     timer_struct[timer_pos].inf.sigev_signo ) ) {
     /* XXX error handling */
  } 

  /*
   * After the signal handler returns, the count of expirations of the
   * timer must be set to 0.
   */

  timer_struct[timer_pos].overrun = 0;

}

/* ********************************************************************* 
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
 * ********************************************************************/

/* **************
 * timer_create
 * **************/

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
)
{

  rtems_status_code return_v;  /* return value of the operation    */
  rtems_id          timer_id;  /* created timer identifier         */
  int               timer_pos; /* Position in the table of timers  */

 /* 
  *  The data of the structure evp are checked in order to verify if they
  *  are coherent. 
  */

  if (evp != NULL) {
    /* The structure has data */
    if ( ( evp->sigev_notify != SIGEV_NONE ) && 
         ( evp->sigev_notify != SIGEV_SIGNAL ) ) {
       /* The value of the field sigev_notify is not valid */
       set_errno_and_return_minus_one( EINVAL );
     }
  }
 
 /*
  *  A timer is created using the primitive rtems_timer_create
  */

  return_v = rtems_timer_create ( clock_id, &timer_id );

  switch (return_v) {
     case RTEMS_SUCCESSFUL : 

       /*
        * The timer has been created properly
        */
 
        /* Obtains the first free position in the table of timers */

        timer_pos = FIRST_FREE_POSITION_F();

        if ( timer_pos == NO_MORE_TIMERS_C ) {
           /* There is not position for another timers in spite of RTEMS 
	    * supports it. It will necessaty to increase the structure used */

           set_errno_and_return_minus_one( EAGAIN );
        }

        /* Exit parameter */

        *timerid  = timer_id;

        /* The data of the created timer are stored to use them later */

        timer_struct[timer_pos].state     = STATE_CREATE_NEW_C;

        /* NEW VERSION*/
        timer_struct[timer_pos].thread_id = pthread_self ();
        
        if ( evp != NULL ) {
           timer_struct[timer_pos].inf.sigev_notify = evp->sigev_notify;
           timer_struct[timer_pos].inf.sigev_signo  = evp->sigev_signo;
           timer_struct[timer_pos].inf.sigev_value  = evp->sigev_value;
        }

        timer_struct[timer_pos].timer_id = timer_id;
        timer_struct[timer_pos].overrun  = 0;

        timer_struct[timer_pos].timer_data.it_value.tv_sec     = 0;
        timer_struct[timer_pos].timer_data.it_value.tv_nsec    = 0;
        timer_struct[timer_pos].timer_data.it_interval.tv_sec  = 0;
        timer_struct[timer_pos].timer_data.it_interval.tv_nsec = 0;

        return 0;

     case RTEMS_INVALID_NAME : /* The assigned name is not valid */

       set_errno_and_return_minus_one( EINVAL );

     case RTEMS_TOO_MANY :

       /* There has been created too much timers for the same process */
       set_errno_and_return_minus_one( EAGAIN );
     
     default :

       /*
        * Does nothing. It only returns the error without assigning a value
        * to errno. In theory, it can not happen because the call to
        * rtems_timer_create can not return other different value. 
        */

       set_errno_and_return_minus_one( EINVAL );
  }

  /* 
   * The next sentence is used to avoid singular situations 
   */

  set_errno_and_return_minus_one( EINVAL );
}

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 */

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

  int               timer_pos;
  rtems_status_code status;


   /* First the position in the table of timers is obtained */

   timer_pos = TIMER_POSITION_F ( timerid );

   if ( timer_pos == BAD_TIMER_C ) {
      /* The timer identifier is erroneus */
      set_errno_and_return_minus_one( EINVAL );
   }

   /* The timer is deleted */

   status = rtems_timer_delete ( timerid );

   if ( status == RTEMS_INVALID_ID ) {
     /* The timer identifier is erroneus */
     set_errno_and_return_minus_one( EINVAL );
   }

   /* Initializes the data of the timer */

   TIMER_INITIALIZE_S ( timer_pos );
   return 0;
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/* **************
 * timer_settime
 * **************/


int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
)
{

   rtems_status_code return_v;   /* Return of the calls to RTEMS        */
   int               timer_pos;  /* Position of the timer in the table  */
   rtems_time_of_day rtems_time; /* Time in RTEMS                       */
   

   /* First the position in the table of timers is obtained */

   timer_pos = TIMER_POSITION_F ( timerid );

   if ( timer_pos == BAD_TIMER_C ) {
     /* The timer identifier is erroneus */
     set_errno_and_return_minus_one( EINVAL );
   }

   if ( value == NULL ) {
     /* The stucture of times of the timer is free, and then returns an
	error but the variable errno is not actualized */

     set_errno_and_return_minus_one( EINVAL );
   }

   /* If the function reaches this point, then it will be necessary to do
    * something with the structure of times of the timer: to stop, start 
    * or start it again */

   /* First, it verifies if the timer must be stopped */

   if ( value->it_value.tv_sec == 0 && value->it_value.tv_nsec == 0 ) {
      /* The timer is stopped */

      return_v = rtems_timer_cancel ( timerid );

      /* The old data of the timer are returned */

      if ( ovalue )
        *ovalue = timer_struct[timer_pos].timer_data;

      /* The new data are set */

      timer_struct[timer_pos].timer_data = *value;

      /* Indicates that the timer is created and stopped */
 
      timer_struct[timer_pos].state = STATE_CREATE_STOP_C;

      /* Returns with success */

      return 0;
   }

   /* 
    * If the function reaches this point, then the timer will have to be
    * initialized with new values: to start it or start it again 
    */
  
   /* First, it verifies if the structure "value" is correct */

    if ( ( value->it_value.tv_nsec > MAX_NSEC_C ) ||
         ( value->it_value.tv_nsec < MIN_NSEC_C ) ) {
       /* The number of nanoseconds is not correct */

       set_errno_and_return_minus_one( EINVAL );
    }

   /* Then, "value" must be converted from seconds and nanoseconds to clock
    * ticks, to use it in the calls to RTEMS */

   /* It is also necessary to take in account if the time is absolute 
    * or relative */

   switch (flags) {
      case TIMER_ABSTIME:

        /* The fire time is absolute:
         * It has to use "rtems_time_fire_when" */

        /* First, it converts from struct itimerspec to rtems_time_of_day */

        ITIMERSPEC_TO_RTEMS_TIME_OF_DAY_S ( value, &rtems_time );

        return_v = rtems_timer_fire_when ( timerid, &rtems_time, FIRE_TIMER_S, NULL);

        switch ( return_v ) {
           case RTEMS_SUCCESSFUL:

              /* The timer has been started and is running */

              /* Actualizes the data of the structure and 
               * returns the old ones in "ovalue" */

              if ( ovalue )
                *ovalue = timer_struct[timer_pos].timer_data;

              timer_struct[timer_pos].timer_data = *value;
 
              /* It indicates that the time is running */

              timer_struct[timer_pos].state = STATE_CREATE_RUN_C;

              /* Stores the time in which the timer was started again */

              timer_struct[timer_pos].time = _TOD_Current;
              return 0;

              break;

           case RTEMS_INVALID_ID:

              /* XXX error handling */
              break;

           case RTEMS_NOT_DEFINED:

              /* XXX error handling */
              break;

           case RTEMS_INVALID_CLOCK:

              /* XXX error handling */
              break;

           default: break;

        
        }
     
        break;

      case TIMER_RELATIVE_C:

        /* The fire time is relative:
         * It has to use "rtems_time_fire_after" */

        /* First, it converts from seconds and nanoseconds to ticks */

        /* The form in which this operation is done can produce a lost 
         * of precision of 1 second */
 
/*      This is the process to convert from nanoseconds to ticks
 *
 *      There is a tick every 10 miliseconds, then the nanoseconds are 
 *      divided between 10**7. The result of this operation will be the
 *      number of ticks 
 */

        timer_struct[timer_pos].ticks = 
             ( SEC_TO_TICKS_C * value->it_value.tv_sec ) +
             ( value->it_value.tv_nsec / ( 1000 * 1000 * 10 ) );

        return_v = rtems_timer_fire_after ( timerid, 
                                           timer_struct[timer_pos].ticks, 
                                           FIRE_TIMER_S, 
                                           NULL );

        switch (return_v) {
           case RTEMS_SUCCESSFUL:

              /* The timer has been started and is running */

              /* Actualizes the data of the structure and 
               * returns the old ones in "ovalue" */

              if ( ovalue )
                *ovalue = timer_struct[timer_pos].timer_data;

              timer_struct[timer_pos].timer_data = *value;
 
              /* It indicates that the time is running */

              timer_struct[timer_pos].state = STATE_CREATE_RUN_C;

              /* Stores the time in which the timer was started again */

              timer_struct[timer_pos].time = _TOD_Current;
 
              return 0;

              break;

           case RTEMS_INVALID_ID:

              /* The timer identifier is not correct. In theory, this 
               * situation can not occur, but the solution is easy */ 

              set_errno_and_return_minus_one( EINVAL );

              break;

           case RTEMS_INVALID_NUMBER:

              /* In this case, RTEMS fails because the values of timing
               * are incorrect */

              /*
               * I do not know if errno must be actualized 
               *
               * errno = EINVAL;
               */

              set_errno_and_return_minus_one( EINVAL );
              break;
           
           default: break;
        }

        break;

      default: break;

        /* It does nothing, although it will be probably necessary to 
         * return an error */
   }

   /* To avoid problems */
   return 0;
}


/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/* **************
 * timer_gettime
 * **************/

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{

 /* 
  * IDEA:  This function does not use functions of RTEMS to the handle
  *        of timers. It uses some functions for managing the time.
  *
  *        A possible form to do this is the following:
  *
  *          - When a timer is initialized, the value of the time in 
  *            that moment is stored.
  *          - When this function is called, it returns the difference
  *            between the current time and the initialization time.
  */
 
  rtems_time_of_day current_time;
  int               timer_pos;
  unsigned32        hours;
  unsigned32        minutes;
  unsigned32        seconds;
  unsigned32        ticks;
  unsigned32        nanosec;
 

  /* Reads the current time */

  current_time = _TOD_Current;

  timer_pos = TIMER_POSITION_F ( timerid );

  if ( timer_pos == BAD_TIMER_C ) {
    /* The timer identifier is erroneus */  
    set_errno_and_return_minus_one( EINVAL );
  }

  /* Calculates the difference between the start time of the timer and
   * the current one */

  hours    = current_time.hour - timer_struct[timer_pos].time.hour;

  if ( current_time.minute < timer_struct[timer_pos].time.minute ) {
     minutes = 60 - timer_struct[timer_pos].time.minute + current_time.minute;
     hours--;
  } else {
     minutes = current_time.minute - timer_struct[timer_pos].time.minute;
  }
    
  if ( current_time.second < timer_struct[timer_pos].time.second ) {
     seconds = 60 - timer_struct[timer_pos].time.second + current_time.second;
     minutes--;
  } else {
     seconds = current_time.second - timer_struct[timer_pos].time.second; 
  }

  if ( current_time.ticks < timer_struct[timer_pos].time.ticks ) {
     ticks = 100 - timer_struct[timer_pos].time.ticks + current_time.ticks;
     seconds--;
  } else {
     ticks = current_time.ticks - timer_struct[timer_pos].time.ticks; 
  }

  /* The time that the timer is running is calculated */
  seconds = hours   * 60 * 60 +
            minutes * 60      +
            seconds; 

  nanosec  = ticks * 10 *  /* msec     */
             1000  *       /* microsec */
             1000;         /* nanosec  */

  
  /* Calculates the time left before the timer finishes */
  
  value->it_value.tv_sec = 
    timer_struct[timer_pos].timer_data.it_value.tv_sec - seconds;
  
  value->it_value.tv_nsec = 
    timer_struct[timer_pos].timer_data.it_value.tv_nsec - nanosec;


  value->it_interval.tv_sec  = 
    timer_struct[timer_pos].timer_data.it_interval.tv_sec;
  value->it_interval.tv_nsec = 
    timer_struct[timer_pos].timer_data.it_interval.tv_nsec;
 

  return 0;

}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

/* *****************
 * timer_getoverrun
 * *****************/

int timer_getoverrun(
  timer_t   timerid
)
{

 /*
  * IDEA: This function must count the times the timer expires.
  *   
  *       The expiration of a timer must increase by one a counter.
  *       After the signal handler associated to the timer finishs 
  *       its execution, FIRE_TIMER_S will have to set this counter to 0.
  */

  int timer_pos; /* Position of the timer in the structure     */
  int overrun;   /* Overflow count                             */


  timer_pos = TIMER_POSITION_F ( timerid );

  if ( timer_pos == BAD_TIMER_C ) {
    /* The timer identifier is erroneus */
    set_errno_and_return_minus_one( EINVAL );
  }

  /* The overflow count of the timer is stored in "overrun" */

  overrun = timer_struct[timer_pos].overrun;

  /* It is set to 0 */

  timer_struct[timer_pos].overrun = 0;

  return overrun;

}
