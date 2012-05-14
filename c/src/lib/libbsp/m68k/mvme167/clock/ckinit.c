/*  ckinit.c
 *
 *  Implementation of the Clock_initialize() functions
 *  prototyped in rtems/c/src/lib/include/clockdrv.h.
 *
 *  This port does not allow the application to select which timer on the
 *  MVME167 to use for the clock, nor does it allow the application to
 *  configure the clock. The clock uses the VMEchip2 Tick Timer #2. This
 *  timer is set up to raise a MC680x0 level-6 interrupt every 1 ms. The
 *  interrupt vector is 0x69.
 *
 *  All page references are to the MVME166/MVME167/MVME187 Single Board
 *  Computer Programmer's Reference Guide (MVME187PG/D2) with the April
 *  1993 supplements/addenda (MVME187PG/D2A1).
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS files:
 *  Copyright (c) 1998, National Research Council of Canada
 */

#include <stdlib.h>
#include <bsp.h>

#define MS_COUNT          1000              /* T2's countdown constant (1 ms) */
#define CLOCK_INT_LEVEL   6                 /* T2's interrupt level */
#define CLOCK_VECTOR (VBR0 * 0x10 + 0x9)    /* T2 is vector $X9 (p. 2-71)*/

/*
 *  These are declared in rtems/c/src/lib/include/clockdrv.h
 *  In other BSPs, rtems_clock_major is set to the largest possible value
 *  (which is almost certainly greater than the number of I/O devices) to
 *  indicate that this device has not been initialized yet. The actual
 *  device number is supplied during initialization. We do not do that.
 *
 *  Initialized data ends up the the .data section. This causes two problems:
 *  1) the .data section is no longer ROMable because we need to write into
 *  it. 2) The initial value is correct only after a download. On subsequent
 *  program restarts, the value is not re-initialized but left to whatever it
 *  was when the previous run terminated or aborted. If we depend on some
 *  global variable value, we must initialize that value explicitly in code
 *  at boot time.
 */
rtems_device_major_number rtems_clock_major;
rtems_device_minor_number rtems_clock_minor;

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the number of
 *  VMEchip2 timer #2 ticks since the driver was initialized.
 */
volatile uint32_t         Clock_driver_ticks;

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of the
 *  RTEMS clock tick routine. This clock tick device driver gets an interrupt
 *  once a millisecond and counts down until the length of time between the
 *  user configured microseconds per tick has passed. This allows the clock
 *  device to "tick" faster than the kernel clock. Of course, the kernel clock
 *  cannot tick faster than the hardware clock. Therefore, the kernel clock
 *  ticks cannot occur more frequently than every 1 millisecond.
 */
uint32_t         Clock_isrs;

/*
 *  Records the previous clock ISR (should be NULL)
 */
rtems_isr_entry  Old_ticker;

/*
 *  Called when the kernel exits.
 */
void clock_exit( void );

/*
 *  VMEchip2_T2_isr
 *
 *  C ISR Handler. Increment the number of internal ticks. If it is time for a
 *  kernel clock tick (if Clock_isrs == 1), call rtems_clock_tick() to signal
 *  the event and reset the Clock_isrs counter; else, just decrement it.
 *
 *  Input parameters:
 *    vector number
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
rtems_isr VMEchip2_T2_isr(
  rtems_vector_number vector
)
{
  char overflow;                /* Content of overflow counter */
  long i;
  long ct;                      /* Number of T2 ticks per RTEMS ticks */

  ct = rtems_configuration_get_microseconds_per_tick() / 1000;

  /*
   *  May have missed interrupts, so should look at the overflow counter.
   */
  lcsr->intr_clear |= 0x02000000;   /* Clear the interrupt */
  overflow = (lcsr->board_ctl >> 12) & 0xF;
  lcsr->board_ctl |= 0x400;         /* Reset overflow counter */

  /* Attempt to protect against one more period */
  if ( overflow == 0 )
    overflow = 16;

  Clock_driver_ticks += overflow;   /* One or more internal ticks */

  if ( Clock_isrs <= overflow ) {
    /* If its time for kernel clock ticks, signal the events to RTEMS */
    for( i = overflow - Clock_isrs; i >= 0; i -= ct ) {
      rtems_clock_tick();
    }
    /* Reset the counter */
    Clock_isrs = (uint32_t)-i;
  }
  else
    Clock_isrs -= overflow;
}

/*
 *  VMEchip2_T2_initialize
 *
 *  Initialize the VMEchip2 Tick Timer #2.
 *
 *  THE VMECHIP2 PRESCALER REGISTER IS ASSUMED TO BE SET!
 *  The prescaler is used by all VMEchip2 timers, including the VMEbus grant
 *  timeout counter, the DMAC time off timer, the DMAC timer on timer, and the
 *  VMEbus global timeout timer. The prescaler value is normally set by the
 *  boot ROM to provide a 1 MHz clock to the timers. For a 25 MHz MVME167, the
 *  prescaler value should be 0xE7 (page 2-63).
 *
 *  Input parameters: NONE
 *
 *  Output paremeters: NONE
 *
 *  Return values: NONE
 */
void VMEchip2_T2_initialize( void )
{
  Clock_driver_ticks = 0;
  Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;

  lcsr->intr_ena &= 0xFDFFFFFF;   /* Disable tick timer 2 interrupt */
  lcsr->intr_clear = 0x02000000;  /* Clear tick timer 2 interrupt */
  lcsr->intr_level[0] =           /* Set tick timer 2 interrupt level */
      (lcsr->intr_level[0] & 0xFFFFFF0F ) | (CLOCK_INT_LEVEL << 4);
  lcsr->timer_cmp_2 = MS_COUNT;   /* Period in compare register */
  lcsr->timer_cnt_2 = 0;          /* Clear tick timer 2 counter */
  Old_ticker =                    /* Install C ISR */
      (rtems_isr_entry) set_vector( VMEchip2_T2_isr, CLOCK_VECTOR, 1 );
  lcsr->board_ctl |= 0x700;       /* Start tick timer 2, reset-on-compare, */
                                  /*  and clear tick timer 2 overflow counter */
  lcsr->intr_ena |= 0x02000000;   /* Enable tick timer 2 interrupt */
  lcsr->vector_base |= 0x00800000;/* Unmask VMEchip2 interrupts */
  atexit( clock_exit );           /* Turn off T2 interrupts when we exit */
}

/*
 *  clock_exit
 *
 *  This routine stops the VMEchip2 T2 timer, disables its interrupt, and
 *  re-install the old interrupt vectors.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */
void clock_exit( void )
{
  lcsr->board_ctl &= 0xFFFFFEFF;  /* Stop tick timer 2 */
  lcsr->intr_ena &= 0xFDFFFFFF;   /* Disable tick timer 2 interrupt */
  lcsr->intr_clear = 0x02000000;  /* Clear tick timer 2 interrupt */

  set_vector( Old_ticker, CLOCK_VECTOR, 1 );
}

/*
 *  Clock_initialize()
 *  prototyped in rtems/c/src/lib/include/clockdrv.h.
 *
 *  Input parameters:
 *     major - console device major number
 *     minor - console device minor number
 *             ALWAYS 0 IN VERSION 3.6.0 OF RTEMS!
 *             Probably provided for symmetry with the other I/O calls.
 *     arg   - pointer to optional device driver arguments
 *             ALWAYS NULL IN VERSION 3.6.0 OF RTEMS!
 *
 *  Output paremeters: NONE
 *
 *  Return values:
 *     rtems_device_driver status code
 */
rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  VMEchip2_T2_initialize();

  /*
   *  Make major/minor avail to others such as shared memory driver
   */
  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
}
