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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <pthread.h>  /* thread facilities */
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <sched.h>    /* schedule facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */



/* temporal parameters of a task */

struct periodic_params {
   struct timespec period;
   int signo;       /* signal number */
   int id;          /* task identification */
 };   

pthread_attr_t attr;

/* shared datum */

struct shared_data {
   pthread_mutex_t mutex;
   pthread_cond_t  sync;
   int updated;
   int x;
 };

struct shared_data data;

/* task A  */

void * task_a (void *arg)
{
   struct   timespec my_period;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;

   my_period = ((struct periodic_params*) arg)->period;
   my_sig    = ((struct periodic_params*) arg)->signo;

   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      pthread_exit ((void *) -1);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   timerdata.it_interval = my_period;
   timerdata.it_value    = my_period;
   if (timer_settime(timer_id, 0, &timerdata, NULL) == -1) {
     perror ("Error in timer setting\n");
     pthread_exit ((void *) -1);
   }

   /* periodic activity */
   while(1) {
     if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
     }
     clock = time(NULL);
     printf("Executing task A %s", ctime(&clock));
   }
   return NULL;
}

/* task B  */

void * task_b (void *arg)
{
   struct   timespec my_period;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;

   int x;   /* value to be copied to the shared datum */

   my_period = ((struct periodic_params*) arg)->period;
   my_sig    = ((struct periodic_params*) arg)->signo;

   x = 1;

   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      pthread_exit ((void *) -1);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   timerdata.it_interval = my_period;
   timerdata.it_value    = my_period;
   if (timer_settime(timer_id, 0, &timerdata, NULL) == -1) {
     perror ("Error in timer setting\n");
     pthread_exit ((void *) -1);
   }

   /* periodic activity */
   while(1) {
     if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
       pthread_exit ((void *) -1);
     }
     pthread_mutex_lock (&data.mutex);
     clock = time(NULL);
     printf("Executing task B with x = %i %s", x, ctime(&clock));
     data.x = x;
     data.updated = TRUE;
     pthread_cond_signal  (&data.sync);
     pthread_mutex_unlock (&data.mutex);
     x++;
   }
   return NULL;
}

/* task C */

void * task_c (void *arg)
{
   struct   timespec my_period;
   int      my_sig, received_sig;
   struct   itimerspec timerdata;
   timer_t  timer_id;
   time_t   clock;
   struct   sigevent event;
   sigset_t set;

   int x;   /* value to be copied to the shared datum */

   my_period = ((struct periodic_params*) arg)->period;
   my_sig    = ((struct periodic_params*) arg)->signo;

   x = 0;

   /* timer create */
   event.sigev_notify = SIGEV_SIGNAL;
   event.sigev_signo = my_sig;
   if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1) {
      perror ("Error in timer creation\n");
      pthread_exit ((void *) -1);
    }

   /* block the timer signal */
   sigemptyset (&set);
   sigaddset (&set,my_sig);
   pthread_sigmask(SIG_BLOCK,&set,NULL);

   /* set the timer in periodic mode */
   timerdata.it_interval = my_period;
   timerdata.it_value    = my_period;
   if (timer_settime(timer_id, 0, &timerdata, NULL) == -1) {
     perror ("Error in timer setting\n");
     pthread_exit ((void *) -1);
   }

   /* periodic activity */
   while(1) {
      if (sigwait(&set,&received_sig) == -1) {
       perror ("Error in sigwait\n");
       pthread_exit ((void *) -1);
     } 
     pthread_mutex_lock (&data.mutex);
     while (data.updated == FALSE) {
       pthread_cond_wait (&data.sync,&data.mutex);
     }
     x = data.x;
     clock = time(NULL);
     printf("Executing task C with x = %i %s", x, ctime(&clock));
     pthread_mutex_unlock (&data.mutex);
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
   pthread_t ta,tb,tc;               /* threads */
   sigset_t  set;                    /* signals */
  
   struct sched_param sch_param;     /* schedule parameters */
   struct periodic_params params_a, params_b, params_c;
   
   puts( "\n\n*** POSIX Timers Test ***" );

   data.updated = FALSE;
   data.x = 0;

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
   if (pthread_attr_setinheritsched (&attr,
     PTHREAD_EXPLICIT_SCHED) != 0) {
      perror("Error in attribute inheritsched\n");
   }

   /* set task independent (join will not use) */
   if (pthread_attr_setdetachstate (&attr,
     PTHREAD_CREATE_DETACHED) != 0) {
      perror ("Error in attribute detachstate\n");
   }

   /* schedule policy POSIX_FIFO (priority preemtive and FIFO within the same
      priority) */
   if (pthread_attr_setschedpolicy (&attr,
     SCHED_FIFO) != 0) {
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
   params_c.signo = SIGALRM;
   if (pthread_create (&tc, &attr, task_c, &params_c) != 0) {
     perror ("Error in trhead create for task c\n");
   }


   /* execute 20 seconds and finish */
   sleep (20);
   puts( "\n\n*** End of POSIX Timers Test ***" );
   rtems_test_exit (0);
 }

