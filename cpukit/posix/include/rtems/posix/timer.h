/**
 * @file rtems/posix/timer.h
 */

/*
 *  $Id$
 */

#ifndef _RTEMS_POSIX_TIMER_H
#define _RTEMS_POSIX_TIMER_H

#include <rtems/posix/config.h>
/*
 * Constants
 */ 

#define STATE_FREE_C        0x01 /* Free position of the table of timers   */
#define STATE_CREATE_NEW_C  0x02 /* Created timer but not running          */
#define STATE_CREATE_RUN_C  0x03 /* Created timer and running              */
#define STATE_CREATE_STOP_C 0x04 /* Created, ran and stopped timer         */
/* Maximum number of nsec allowed         */
#define MAX_NSEC_C    (uint32_t  )1000000000
#define MIN_NSEC_C             0 /* Minimum number of nsec allowew         */
#define TIMER_RELATIVE_C       0 /* Indicates that the fire time is
                                  * relative to the current one            */
#define SEC_TO_TICKS_C _TOD_Ticks_per_second /* Number of ticks in a second*/
/* Nanoseconds in a second               */
#define NSEC_PER_SEC_C (uint32_t  )1000000000

#define SECONDS_PER_YEAR_C    (uint32_t  )(360 * 24) * (uint32_t  )(60 * 60)
#define SECONDS_PER_MONTH_C   (uint32_t  )( 30 * 24) * (uint32_t  )(60 * 60)
#define SECONDS_PER_DAY_C     (uint32_t  )( 24 * 60) * (uint32_t  )(60)
#define SECONDS_PER_HOUR_C               (uint32_t  )( 60 * 60 )
#define SECONDS_PER_MINUTE_C                  (uint32_t  )( 60 )


/*
 * Data for a timer
 */

typedef struct {
  Objects_Control   Object;
  Watchdog_Control  Timer;      /* Internal Timer                        */
  pthread_t         thread_id;  /* Thread identifier                     */
  char              state;      /* State of the timer                    */
  struct sigevent   inf;        /* Information associated to the timer   */
  struct itimerspec timer_data; /* Timing data of the timer              */
  uint32_t          ticks;      /* Number of ticks of the initialization */
  uint32_t          overrun;    /* Number of expirations of the timer    */
  struct timespec   time;       /* Time in which the timer was started   */
} POSIX_Timer_Control;

/*
 *  _POSIX_Timers_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Timer_Manager_initialization ( int max_timers );

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Timer_Information;

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/timer.inl>
#endif

#endif
/* end of include file */
