/*  Clock
 *
 *  This routine generates clock ticks using standard POSIX services.
 *  The tick frequency is specified by the bsp.
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
#include <rtems/libio.h>
#include <bsp.h>

/*
 *  In order to get the types and prototypes used in this file under
 *  Solaris 2.3, it is necessary to pull the following magic.
 */
 
#if defined(solaris)
#warning "Ignore the undefining __STDC__ warning"
#undef __STDC__
#define __STDC__ 0
#undef  _POSIX_C_SOURCE
#endif
 
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

extern rtems_configuration_table Configuration;

void Clock_exit(void);

volatile rtems_unsigned32 Clock_driver_ticks;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

void
Install_clock(rtems_isr_entry clock_isr)
{
    struct itimerval  new;

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
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
    rtems_unsigned32  isrlevel = 0;

    rtems_interrupt_disable(isrlevel);
    (void)set_vector(new_clock_isr, SIGALRM, 1);
    rtems_interrupt_enable(isrlevel);
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
    struct itimerval  new;
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

rtems_device_driver
Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    Install_clock((rtems_isr_entry) Clock_isr);

    /*
     * make major/minor avail to others such as shared memory driver
     */
    rtems_clock_major = major;
    rtems_clock_minor = minor;

    return RTEMS_SUCCESSFUL;
}

rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    rtems_libio_ioctl_args_t *args = pargp;

    if (args == 0)
        goto done;

    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */

    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(SIGALRM);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
        ReInstall_clock(args->buffer);
    }
    
done:
    return RTEMS_SUCCESSFUL;
}
