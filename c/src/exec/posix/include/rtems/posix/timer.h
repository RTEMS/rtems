/*
 *  $Id$
 */

#ifndef __RTEMS_POSIX_TIMERS_h
#define __RTEMS_POSIX_TIMERS_h

/*
 * Data for a timer
 */

typedef struct {
  pthread_t         thread_id;  /* Thread identifier                     */
  char              state;      /* State of the timer                    */
  struct sigevent   inf;        /* Information associated to the timer   */
  timer_t           timer_id;   /* Created timer identifier              */
  struct itimerspec timer_data; /* Timing data of the timer              */
  unsigned32        ticks;      /* Number of ticks of the initialization */
  unsigned32        overrun;    /* Number of expirations of the timer    */
  rtems_time_of_day time;       /* Time in which the timer was started   */
} timer_alive_t;

/*
 * Array of Timers
 */

extern int timer_max;
extern timer_alive_t *timer_struct;

#endif
/* end of include file */

