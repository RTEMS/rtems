/*  Clock
 *
 *  This routine initializes the interval timer on the
 *  PA-RISC CPU.  The tick frequency is specified by the bsp.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

extern rtems_configuration_table Configuration;
extern sigset_t                  UNIX_SIGNAL_MASK;

/*
 * Function prototypes
 */

void Install_clock();
void Clock_isr();
void Clock_exit();

/*
 * CPU_HPPA_CLICKS_PER_TICK is either a #define or an rtems_unsigned32
 *   allocated and set by bsp_start()
 */

#ifndef CPU_HPPA_CLICKS_PER_TICK
extern rtems_unsigned32 CPU_HPPA_CLICKS_PER_TICK;
#endif

volatile rtems_unsigned32 Clock_driver_ticks;

struct itimerval  new;

rtems_device_driver
Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id tid,
  rtems_unsigned32 *rval
)
{
    Install_clock(Clock_isr);
}

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
    rtems_unsigned32  isrlevel = 0;

    rtems_interrupt_disable(isrlevel);
    (void)set_vector(new_clock_isr, SIGALRM, 1);
    rtems_interrupt_enable(isrlevel);
}

void
Install_clock(rtems_isr_entry clock_isr)
{
    Clock_driver_ticks = 0;

    new.it_value.tv_sec = 0;
    new.it_value.tv_usec = Configuration.microseconds_per_tick;
    new.it_interval.tv_sec = 0;
    new.it_interval.tv_usec = Configuration.microseconds_per_tick;

    (void)set_vector(clock_isr, SIGALRM, 1);

    setitimer(ITIMER_REAL, &new, 0);

    atexit(Clock_exit);
}

void
Clock_isr(int vector)
{
    Clock_driver_ticks++;

    rtems_clock_tick();
}

/*
 * Called via atexit()
 * Remove the clock signal
 */

void
Clock_exit(void)
{
     struct sigaction  act;

     /*
      * Set the SIGALRM signal to ignore any last
      * signals that might come in while we are
      * disarming the timer and removing the interrupt
      * vector.
      */

     act.sa_handler = SIG_IGN;

     sigaction(SIGALRM, &act, 0);

    new.it_value.tv_sec = 0;
    new.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &new, 0);

    (void)set_vector(0, SIGALRM, 1);
}
