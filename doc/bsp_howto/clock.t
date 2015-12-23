@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Clock Driver

@section Introduction

The purpose of the clock driver is to provide two services for the operating
system.
@itemize @bullet
@item A steady time basis to the kernel, so that the RTEMS primitives that need
a clock tick work properly.  See the @cite{Clock Manager} chapter of the
@cite{RTEMS Application C User's Guide} for more details.
@item An optional time counter to generate timestamps of the uptime and wall
clock time.
@end itemize

The clock driver is usually located in the @file{clock} directory of the BSP.
Clock drivers should use the @dfn{Clock Driver Shell} available via the
@file{clockdrv_shell.h} include file.

@section Clock Driver Shell

The @dfn{Clock Driver Shell} include file defines the clock driver functions
declared in @code{#include <rtems/clockdrv.h>} which are used by RTEMS
configuration file @code{#include <rtems/confdefs.h>}.  In case the application
configuration defines @code{#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER},
then the clock driver is registered and should provide its services to the
operating system.  A hardware specific clock driver must provide some
functions, defines and macros for the @dfn{Clock Driver Shell} which are
explained here step by step.  A clock driver file looks in general like this.

@example
/*
 * A section with functions, defines and macros to provide hardware specific
 * functions for the Clock Driver Shell.
 */

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection Initialization

Depending on the hardware capabilities one out of three clock driver variants
must be selected.
@itemize @bullet
@item The most basic clock driver provides only a periodic interrupt service
routine which calls @code{rtems_clock_tick()}.  The interval is determined by
the application configuration via @code{#define
CONFIGURE_MICROSECONDS_PER_TICK} and can be obtained via
@code{rtems_configuration_get_microseconds_per_tick()}.  The timestamp
resolution is limited to the clock tick interval.
@item In case the hardware lacks support for a free running counter, then the
module used for the clock tick may provide support for timestamps with a
resolution below the clock tick interval.  For this so called simple
timecounters can be used.
@item The desired variant uses a free running counter to provide accurate
timestamps.  This variant is mandatory on SMP configurations.
@end itemize

@subsubsection Clock Tick Only Variant

@example
static void some_support_initialize_hardware( void )
@{
  /* Initialize hardware */
@}

#define Clock_driver_support_initialize_hardware() \
  some_support_initialize_hardware()

/* Indicate that this clock driver lacks a proper timecounter in hardware */
#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/clockdrv_shell.h"
@end example

@subsubsection Simple Timecounter Variant

@example
#include <rtems/timecounter.h>

static rtems_timecounter_simple some_tc;

static uint32_t some_tc_get( rtems_timecounter_simple *tc )
@{
  return some.counter;
@}

static void some_tc_at_tick( rtems_timecounter_simple *tc )
@{
  /*
   * Do work necessary at the clock tick interrupt, e.g. clear a pending flag.
   */
@}

static bool some_tc_is_pending( rtems_timecounter_simple *tc )
@{
  return some.is_pending;
@}

static uint32_t some_tc_get_timecount( struct timecounter *tc )
@{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    some_tc_get,
    some_tc_is_pending
  );
@}

static void some_tc_tick( void )
@{
  rtems_timecounter_simple_downcounter_tick(
    &some_tc,
    some_tc_get,
    some_tc_at_tick
  );
@}

static void some_support_initialize_hardware( void )
@{
  uint32_t frequency = 123456;
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t timecounter_ticks_per_clock_tick =
    ( frequency * us_per_tick ) / 1000000;

  /* Initialize hardware */

  rtems_timecounter_simple_install(
    &some_tc,
    frequency,
    timecounter_ticks_per_clock_tick,
    some_tc_get_timecount
  );
@}

#define Clock_driver_support_initialize_hardware() \
  some_support_initialize_hardware()

#define Clock_driver_timecounter_tick() \
  some_tc_tick()

#include "../../../shared/clockdrv_shell.h"
@end example

@subsubsection Timecounter Variant

This variant is preferred since it is the most efficient and yields the most
accurate timestamps.  It is also mandatory on SMP configurations to obtain
valid timestamps.  The hardware must provide a periodic interrupt to service
the clock tick and a free running counter for the timecounter.  The free
running counter must have a power of two period.  The @code{tc_counter_mask}
must be initialized to the free running counter period minus one, e.g. for a
32-bit counter this is 0xffffffff.  The @code{tc_get_timecount} function must
return the current counter value (the counter values must increase, so if the
counter counts down, a conversion is necessary).  Use
@code{RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER} for the @code{tc_quality}.  Set
@code{tc_frequency} to the frequency of the free running counter in Hz.  All
other fields of the @code{struct timecounter} must be zero initialized.
Install the initialized timecounter via @code{rtems_timecounter_install()}.

@example
#include <rtems/timecounter.h>

static struct timecounter some_tc;

static uint32_t some_tc_get_timecount( struct timecounter *tc )
@{
  some.free_running_counter;
@}

static void some_support_initialize_hardware( void )
@{
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t frequency = 123456;

  /*
   * The multiplication must be done in 64-bit arithmetic to avoid an integer
   * overflow on targets with a high enough counter frequency.
   */
  uint32_t interval = (uint32_t) ( ( frequency * us_per_tick ) / 1000000 );

  /*
   * Initialize hardware and set up a periodic interrupt for the configuration
   * based interval.
   */

  some_tc.tc_get_timecount = some_tc_get_timecount;
  some_tc.tc_counter_mask = 0xffffffff;
  some_tc.tc_frequency = frequency;
  some_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install( &some_tc );
@}

#define Clock_driver_support_initialize_hardware() \
  some_support_initialize_hardware()

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection Install Clock Tick Interrupt Service Routine

The clock driver must provide a function to install the clock tick interrupt
service routine via @code{Clock_driver_support_install_isr()}.

@example
#include <bsp/irq.h>
#include <bsp/fatal.h>

static void some_support_install_isr( rtems_interrupt_handler isr )
@{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    SOME_IRQ,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) @{
    bsp_fatal( SOME_FATAL_IRQ_INSTALL );
  @}
@}

#define Clock_driver_support_install_isr( isr, old ) \
  some_support_install_isr( isr )

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection Support At Tick

The hardware specific support at tick is specified by
@code{Clock_driver_support_at_tick()}.

@example
static void some_support_at_tick( void )
@{
  /* Clear interrupt */
@}

#define Clock_driver_support_at_tick() \
  some_support_at_tick()

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection System Shutdown Support

The @dfn{Clock Driver Shell} provides the routine @code{Clock_exit()} that is
scheduled to be run during system shutdown via the @code{atexit()} routine.
The hardware specific shutdown support is specified by
@code{Clock_driver_support_shutdown_hardware()} which is used by
@code{Clock_exit()}.  It should disable the clock tick source if it was
enabled.  This can be used to prevent clock ticks after the system is shutdown.

@example
static void some_support_shutdown_hardware( void )
@{
  /* Shutdown hardware */
@}

#define Clock_driver_support_shutdown_hardware() \
  some_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection Multiple Clock Driver Ticks Per Clock Tick

In case the hardware needs more than one clock driver tick per clock tick (e.g.
due to a limited range of the hardware timer), then this can be specified with
the optional @code{#define CLOCK_DRIVER_ISRS_PER_TICK} and @code{#define
CLOCK_DRIVER_ISRS_PER_TICK_VALUE} defines.  This is currently used only for x86
and it hopefully remains that way.

@example
/* Enable multiple clock driver ticks per clock tick */
#define CLOCK_DRIVER_ISRS_PER_TICK 1

/* Specifiy the clock driver ticks per clock tick value */
#define CLOCK_DRIVER_ISRS_PER_TICK_VALUE 123

#include "../../../shared/clockdrv_shell.h"
@end example

@subsection Clock Driver Ticks Counter

The @dfn{Clock Driver Shell} provide a global variable that is simply a count
of the number of clock driver interrupt service routines that have occurred.
This information is valuable when debugging a system.  This variable is
declared as follows:

@example
volatile uint32_t Clock_driver_ticks;
@end example
