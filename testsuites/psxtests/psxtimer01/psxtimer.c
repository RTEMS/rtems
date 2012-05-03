/*
 *
 *  This is a simple real-time applications which contains 3 periodic tasks.
 *
 *  Task A is an independent task.
 *
 *  Task B and C share a data.
 *
 *  Tasks are implemented as POSIX threads.
 *
 *  The share data is protected with a POSIX mutex.
 *
 *  Other POSIX facilities such as timers, condition, .. is also used
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <pthread.h>  /* thread facilities */
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <sched.h>    /* schedule facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */
#include <rtems/score/timespec.h>
#include "pritime.h"

void StopTimer(
  timer_t  timer_id,
  struct   itimerspec *timerdata
);

/* temporal parameters of a task */

struct periodic_params {
   struct timespec period;
   int count;       /* Number of iterations to run */
   int signo;       /* signal number */
   int id;          /* task identification */
 };

pthread_attr_t attr;

/* shared datum */

struct shared_data {
   pthread_mutex_t mutex;
   pthread_cond_t  sync;
   int updated;
 };

struct shared_data data;

void StopTimer(
  timer_t  timer_id,
  struct   itimerspec *timerdata
)
{
  static int         firstTime = 1;
  struct itimerspec *pOld;
  struct itimerspec  odata;

  /*
   *  We do not care about the old value.  But we need to exercise
   *  getting and not getting the return value back.
   */
  pOld = (firstTime == 1) ? NULL : &odata;
  firstTime = 0;

  timerdata->it_value.tv_sec  = 0;
  timerdata->it_value.tv_nsec  = 0;
  if (timer_settime(timer_id,POSIX_TIMER_RELATIVE,timerdata,pOld) == -1) {
    perror ("Error in timer setting\n");
    rtems_test_exit(0);
  }
}

/* task A  */
void * task_a (void *arg)
{
   struct   timespec my_period;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   struct   itimerspec timergetdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;
   struct periodic_params *params;

   params = arg;
   my_period = params->period;
   my_sig    = params->signo;

   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      rtems_test_exit(0);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   timerdata.it_interval = my_period;
   timerdata.it_value    = my_period;
   timerdata.it_value.tv_sec *= 2;
   if (timer_settime(timer_id,POSIX_TIMER_RELATIVE,&timerdata,&timergetdata) == -1) {
     perror ("Error in timer setting\n");
     rtems_test_exit(0);
   }
   printf(
    "task A: timer_settime - value=%" PRIdtime_t ":%ld interval=%" PRIdtime_t ":%ld\n",
    timergetdata.it_value.tv_sec, timergetdata.it_value.tv_nsec,
    timergetdata.it_interval.tv_sec, timergetdata.it_interval.tv_nsec
  );


   /* periodic activity */
   while(1) {
     if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
     }
     if (timer_gettime(timer_id, &timerdata) == -1) {
       perror ("Error in timer_gettime\n");
       rtems_test_exit(0);
     }
     if (! _Timespec_Equal_to( &timerdata.it_value, &my_period )){
       perror ("Error in Task A timer_gettime\n");
     }
     clock = time(NULL);
     printf("Executing task A with count = %2i %s", params->count, ctime(&clock));
     params->count--;
     if (params->count == 0)
       StopTimer(timer_id, &timerdata);
   }
   return NULL;
}

/* task B  */

void * task_b (void *arg)
{
   struct   timespec my_period;
   struct   timespec now;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;
   struct periodic_params *params;

   params = arg;
   my_period = params->period;
   my_sig    = params->signo;


   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      rtems_test_exit(0);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   clock_gettime( CLOCK_REALTIME, &now );
   timerdata.it_interval = my_period;
   timerdata.it_value = now;
   _Timespec_Add_to( &timerdata.it_value, &my_period );
   if (timer_settime(timer_id,TIMER_ABSTIME,&timerdata,NULL) == -1) {
     perror ("Error in timer setting\n");
     rtems_test_exit(0);
   }

   /* periodic activity */
   while(1) {
     if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
       rtems_test_exit(0);
     }

     if (timer_gettime(timer_id, &timerdata) == -1) {
       perror ("Error in timer_gettime\n");
       rtems_test_exit(0);
     }

#if 0
     /*
      *  It is not an error if they are not equal.  A clock tick could occur
      *  and thus they are close but not equal.  Can we test for this?
      */
     if ( !_Timespec_Equal_to( &timerdata.it_value, &my_period) ){
       printf( "NOT EQUAL %d:%d != %d:%d\n",
          timerdata.it_value.tv_sec,
          timerdata.it_value.tv_nsec,
          my_period.tv_sec,
          my_period.tv_nsec
       );
       rtems_test_exit(0);
     }
#endif

     pthread_mutex_lock (&data.mutex);
     clock = time(NULL);
     printf("Executing task B with count = %2i %s",
       params->count, ctime(&clock)
     );
     data.updated = TRUE;
     pthread_cond_signal  (&data.sync);
     pthread_mutex_unlock (&data.mutex);
     params->count--;
     if (params->count == 0)
       StopTimer(timer_id, &timerdata);
   }
   return NULL;
}

/* task C */

void * task_c (void *arg)
{
   int      count;
   struct   timespec my_period;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   struct   itimerspec timergetdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;
   struct   periodic_params *params;

   params = arg;
   my_period = params->period;
   my_sig    = params->signo;

   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      rtems_test_exit(0);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   timerdata.it_interval = my_period;
   timerdata.it_value    = my_period;
   timerdata.it_value.tv_sec *= 2;
   if (timer_settime(timer_id,POSIX_TIMER_RELATIVE,&timerdata,NULL) == -1) {
     perror ("Error in timer setting\n");
     rtems_test_exit(0);
   }

   /* periodic activity */
   for (count=0 ; ; count++) {
      if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
       rtems_test_exit(0);
     }
     if (timer_gettime(timer_id, &timerdata) == -1) {
       perror ("Error in timer_gettime\n");
       rtems_test_exit(0);
     }
     if (! _Timespec_Equal_to( &timerdata.it_value, &my_period) ){
       perror ("Error in Task C timer_gettime\n");
     }
     pthread_mutex_lock (&data.mutex);
     while (data.updated == FALSE) {
       pthread_cond_wait (&data.sync,&data.mutex);
     }
     clock = time(NULL);
     printf("Executing task C with count = %2i %s",
       params->count, ctime(&clock)
     );

     if ( count && (count % 5) == 0 ) {
       int overruns = 0;
       sleep(1);
       overruns = timer_getoverrun( timer_id );
       printf( "task C: timer_getoverrun - overruns=%d\n", overruns );

       if (timer_gettime(timer_id, &timergetdata) == -1) {
	 perror ("Error in timer setting\n");
         rtems_test_exit(0);
       }
       printf(
         "task C: timer_gettime - %" PRIdtime_t ":%ld remaining from %" PRIdtime_t ":%ld\n",
         timergetdata.it_value.tv_sec, timergetdata.it_value.tv_nsec,
         timergetdata.it_interval.tv_sec, timergetdata.it_interval.tv_nsec
       );
     }

     pthread_mutex_unlock (&data.mutex);
     params->count--;
     if (params->count == 0)
       StopTimer(timer_id, &timerdata);
   }
   return NULL;
}

/* main */

void *POSIX_Init (
  void *argument
)

{
   pthread_mutexattr_t mutexattr;    /* mutex attributes */
   pthread_condattr_t  condattr;     /* condition attributes */
   pthread_attr_t attr;              /* task attributes */
   pthread_t ta,tb,tc, tc1;          /* threads */
   sigset_t  set;                    /* signals */

   struct sched_param sch_param;     /* schedule parameters */
   struct periodic_params params_a, params_b, params_c, params_c1;

   puts( "\n\n*** POSIX Timers Test 01 ***" );

   data.updated = FALSE;

   /* mask signal */
   sigemptyset (&set);
   sigaddset (&set,SIGALRM);
   pthread_sigmask (SIG_BLOCK,&set,NULL);

   /* set mutex attributes */
   if (pthread_mutexattr_init (&mutexattr) != 0) {
     perror ("Error in mutex attribute init\n");
   }

   /* init mutex */
   if (pthread_mutex_init (&data.mutex,&mutexattr) != 0) {
     perror ("Error in mutex init");
   }

    /* init condition attributes */
   if (pthread_condattr_init (&condattr) != 0) {
     perror ("Error in condition attribute init\n");
   }

   /* init condition */
   if (pthread_cond_init (&data.sync,&condattr) != 0) {
     perror ("Error in condition init");
   }

   /* init task attributes */
   if (pthread_attr_init(&attr) != 0) {
     perror ("Error in attribute init\n");
   }

   /* set explicit schedule for every task */
   if (pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
      perror("Error in attribute inheritsched\n");
   }

   /* set task independent (join will not use) */
   if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED) != 0) {
      perror ("Error in attribute detachstate\n");
   }

   /* schedule policy POSIX_FIFO (priority preemtive and FIFO within the same
      priority) */
   if (pthread_attr_setschedpolicy (&attr, SCHED_FIFO) != 0) {
      perror ("Error in attribute setschedpolicy\n");
    }

   /* set and create thread A with priority 1 */

   sch_param.sched_priority = 1;
   if (pthread_attr_setschedparam(&attr, &sch_param) != 0) {
      perror ("Error in attribute schedparam\n");
    }

   /* Temporal parameters (1 sec. periodicity) */

   params_a.period.tv_sec  = 1;         /* seconds */
   params_a.period.tv_nsec = 000000000; /* nanoseconds */
   params_a.count          = 20;
   params_a.signo = SIGALRM;
   if (pthread_create (&ta, &attr, task_a, &params_a) != 0 ) {
     perror ("Error in thread create for task a\n");
   }

   /* set and create thread B with priority 15 */

   sch_param.sched_priority = 15;
   if (pthread_attr_setschedparam(&attr, &sch_param) != 0) {
      perror ("Error in attribute schedparam");
    }

   /* Temporal parameters (2 sec. periodicity) */
   params_b.period.tv_sec  = 2;         /* seconds */
   params_b.period.tv_nsec = 000000000; /* nanoseconds */
   params_b.count          = 10;
   params_b.signo = SIGALRM;
   if (pthread_create (&tb, &attr, task_b, &params_b) != 0) {
     perror ("Error in thread create for task b\n");
   }

   /* set and create thread B with priority 14 */

   sch_param.sched_priority = 14;
   if (pthread_attr_setschedparam(&attr, &sch_param) != 0 ) {
      perror ("Error in attribute schedparam\n");
    }

   /* Temporal parameters (3 sec. periodicity) */
   params_c.period.tv_sec  = 3;         /* seconds */
   params_c.period.tv_nsec = 000000000; /* nanoseconds */
   params_c.count          = 6;
   params_c.signo = SIGALRM;
   if (pthread_create (&tc, &attr, task_c, &params_c) != 0) {
     perror ("Error in thread create for task c\n");
   }

   /* execute 25 seconds and finish */
   sleep (25);

   puts( "starting C again with 0.5 second periodicity" );
   /* Temporal parameters (0.5 sec. periodicity) */
   params_c1.period.tv_sec  = 0;         /* seconds */
   params_c1.period.tv_nsec = 500000000; /* nanoseconds */
   params_c1.count          = 6;
   params_c1.signo = SIGALRM;
   if (pthread_create (&tc1, &attr, task_c, &params_c1) != 0) {
     perror ("Error in thread create for task c1\n");
   }

   sleep(5);

   puts( "*** END OF POSIX Timers Test 01 ***" );
   rtems_test_exit (0);
 }
