/*
 *  $Id$
 */

#ifndef __RTEMS_POSIX_TIMERS_h
#define __RTEMS_POSIX_TIMERS_h

/* ************
 * Constants
 * ************/

#define STATE_FREE_C        0x01 /* Free position of the table of timers   */
#define STATE_CREATE_NEW_C  0x02 /* Created timer but not running          */
#define STATE_CREATE_RUN_C  0x03 /* Created timer and running              */
#define STATE_CREATE_STOP_C 0x04 /* Created, ran and stopped timer         */
/* Maximum number of nsec allowed         */
#define MAX_NSEC_C    (unsigned32)1000000000
#define MIN_NSEC_C             0 /* Minimum number of nsec allowew         */
#define TIMER_RELATIVE_C       0 /* Indicates that the fire time is
                                  * relative to the current one            */
#define SEC_TO_TICKS_C _TOD_Ticks_per_second /* Number of ticks in a second*/
/* Nanoseconds in a second               */
#define NSEC_PER_SEC_C (unsigned32)1000000000

#define NO_MORE_TIMERS_C      11 /* There is not available timers          */
#define BAD_TIMER_C           11 /* The timer does not exist in the table  */

#define SECONDS_PER_YEAR_C    (unsigned32)(360 * 24) * (unsigned32)(60 * 60)
#define SECONDS_PER_MONTH_C   (unsigned32)( 30 * 24) * (unsigned32)(60 * 60)
#define SECONDS_PER_DAY_C     (unsigned32)( 24 * 60) * (unsigned32)(60)
#define SECONDS_PER_HOUR_C               (unsigned32)( 60 * 60 )
#define SECONDS_PER_MINUTE_C                  (unsigned32)( 60 )


/*
 * Data for a timer
 */

typedef struct {
  Objects_Control   Object;
  Watchdog_Control  Ticker;

  pthread_t         thread_id;  /* Thread identifier                     */
  char              state;      /* State of the timer                    */
  struct sigevent   inf;        /* Information associated to the timer   */
  timer_t           timer_id;   /* Created timer identifier              */
  struct itimerspec timer_data; /* Timing data of the timer              */
  unsigned32        ticks;      /* Number of ticks of the initialization */
  unsigned32        overrun;    /* Number of expirations of the timer    */
  rtems_time_of_day time;       /* Time in which the timer was started   */
} POSIX_Timer_Control;

/*
 * Array of Timers
 */

extern int timer_max;
extern POSIX_Timer_Control *timer_struct;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

RTEMS_EXTERN Objects_Information  _POSIX_Timer_Information;

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/timer.inl>
#endif

#endif
/* end of include file */

