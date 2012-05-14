/*
 * This file contains the generic RTEMS clock driver the Hitachi SH 7750
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 2001
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/sh.h>
#include <rtems/score/ispsh7750.h>
#include <rtems/score/iosh7750.h>

extern uint32_t bsp_clicks_per_second;

#ifndef CLOCKPRIO
#define CLOCKPRIO 10
#endif

/* Clock timer prescaler division ratio */
#define CLOCK_PRESCALER 4
#define TCR0_TPSC       SH7750_TCR_TPSC_DIV4

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR SH7750_EVT_TO_NUM(SH7750_EVT_TUNI0)

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile uint32_t   Clock_driver_ticks;

static void Clock_exit( void );
static rtems_isr Clock_isr( rtems_vector_number vector );

/*
 * These are set by clock driver during its init
 */
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

/*
 *  Isr Handler
 */

/* Clock_isr --
 *     Clock interrupt handling routine.
 *
 * PARAMETERS:
 *     vector - interrupt vector number
 *
 * RETURNS:
 *     none
 */
rtems_isr
Clock_isr(rtems_vector_number vector)
{
    uint16_t   tcr;

    /* reset the timer underflow flag */
    tcr = read16(SH7750_TCR0);
    write16(tcr & ~SH7750_TCR_UNF, SH7750_TCR0);

    /* Increment the clock interrupt counter */
    Clock_driver_ticks++ ;

    /* Invoke rtems clock service routine */
    rtems_clock_tick();
}

/* Install_clock --
 *     Install a clock tick handler and reprograms the chip.  This
 *     is used to initially establish the clock tick.
 *
 * PARAMETERS:
 *     clock_isr - Clock interrupt stay routine
 *
 * RETURNS:
 *     none
 *
 * SIDE EFFECTS:
 *     Establish clock interrupt handler, configure Timer 0 hardware
 */
void
Install_clock(rtems_isr_entry clock_isr)
{
    int cpudiv = 1; /* CPU frequency divider */
    int tidiv = 1;  /* Timer input frequency divider */
    uint32_t   timer_divider; /* Calculated Timer Divider value */
    uint8_t   temp8;
    uint16_t   temp16;

    /*
     *  Initialize the clock tick device driver variables
     */

    Clock_driver_ticks = 0;

    /* Get CPU frequency divider from clock unit */
    switch (read16(SH7750_FRQCR) & SH7750_FRQCR_IFC)
    {
        case SH7750_FRQCR_IFCDIV1:
            cpudiv = 1;
            break;

        case SH7750_FRQCR_IFCDIV2:
            cpudiv = 2;
            break;

        case SH7750_FRQCR_IFCDIV3:
            cpudiv = 3;
            break;

        case SH7750_FRQCR_IFCDIV4:
            cpudiv = 4;
            break;

        case SH7750_FRQCR_IFCDIV6:
            cpudiv = 6;
            break;

        case SH7750_FRQCR_IFCDIV8:
            cpudiv = 8;
            break;

        default:
            rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);
    }

    /* Get peripheral module frequency divider from clock unit */
    switch (read16(SH7750_FRQCR) & SH7750_FRQCR_PFC)
    {
        case SH7750_FRQCR_PFCDIV2:
            tidiv = 2 * CLOCK_PRESCALER;
            break;

        case SH7750_FRQCR_PFCDIV3:
            tidiv = 3 * CLOCK_PRESCALER;
            break;

        case SH7750_FRQCR_PFCDIV4:
            tidiv = 4 * CLOCK_PRESCALER;
            break;

        case SH7750_FRQCR_PFCDIV6:
            tidiv = 6 * CLOCK_PRESCALER;
            break;

        case SH7750_FRQCR_PFCDIV8:
            tidiv = 8 * CLOCK_PRESCALER;
            break;

        default:
            rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);
    }
    timer_divider =
        (bsp_clicks_per_second * cpudiv / (tidiv*1000000)) *
        rtems_configuration_get_microseconds_per_tick();

    /*
     *  Hardware specific initialization
     */

    /* Stop the Timer 0 */
    temp8 = read8(SH7750_TSTR);
    temp8 &= ~SH7750_TSTR_STR0;
    write8(temp8, SH7750_TSTR);

    /* Establish interrupt handler */
    rtems_interrupt_catch( Clock_isr, CLOCK_VECTOR, &Old_ticker );

    /* Reset counter */
    write32(timer_divider, SH7750_TCNT0);

    /* Load divider */
    write32(timer_divider, SH7750_TCOR0);

    write16(
        SH7750_TCR_UNIE |        /* Enable Underflow Interrupt */
        SH7750_TCR_CKEG_RAISE |  /* Count on rising edge */
        TCR0_TPSC,               /* Timer prescaler ratio */
        SH7750_TCR0);

    /* Set clock interrupt priority */
    temp16 = read16(SH7750_IPRA);
    temp16 = (temp16 & ~SH7750_IPRA_TMU0) | (CLOCKPRIO << SH7750_IPRA_TMU0_S);
    write16(temp16, SH7750_IPRA);

    /* Start the Timer 0 */
    temp8 = read8(SH7750_TSTR);
    temp8 |= SH7750_TSTR_STR0;
    write8(temp8, SH7750_TSTR);

    /*
     *  Schedule the clock cleanup routine to execute if the application exits.
     */

    atexit( Clock_exit );
}

/* Clock_exit --
 *     Clean up before the application exits
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 *
 * SIDE EFFECTS:
 *     Stop Timer 0 counting, set timer 0 interrupt priority level to 0.
 */
void
Clock_exit(void)
{
    uint8_t   temp8 = 0;
    uint16_t   temp16 = 0;

    /* turn off the timer interrupts */
    /* Stop the Timer 0 */
    temp8 = read8(SH7750_TSTR);
    temp8 &= ~SH7750_TSTR_STR0;
    write8(temp8, SH7750_TSTR);

    /* Lower timer interrupt priority to 0 */
    temp16 = read16(SH7750_IPRA);
    temp16 = (temp16 & ~SH7750_IPRA_TMU0) | (0 << SH7750_IPRA_TMU0_S);
    write16(temp16, SH7750_IPRA);

  /* old vector shall not be installed */
}

/* Clock_initialize --
 *     Device driver entry point for clock tick driver initialization.
 *
 * PARAMETERS:
 *     major - clock major device number
 *     minor - clock minor device number
 *     pargp - driver initialize primitive argument, not used
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_device_driver
Clock_initialize(rtems_device_major_number major,
                 rtems_device_minor_number minor,
                 void *pargp)
{
    Install_clock( Clock_isr );

    /*
     * make major/minor avail to others such as shared memory driver
     */
    rtems_clock_major = major;
    rtems_clock_minor = minor;

    return RTEMS_SUCCESSFUL;
}
