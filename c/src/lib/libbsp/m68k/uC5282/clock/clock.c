/*
 * Use the last periodic interval timer (PIT3) as the system clock.
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5282/mcf5282.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+58)

/*
 * CPU load counters
 * Place in static RAM so updates don't hit the SDRAM
 */
#define IDLE_COUNTER    __SRAMBASE.idle_counter
#define FILTERED_IDLE   __SRAMBASE.filtered_idle
#define MAX_IDLE_COUNT  __SRAMBASE.max_idle_count
#define PITC_PER_TICK   __SRAMBASE.pitc_per_tick
#define NSEC_PER_PITC   __SRAMBASE.nsec_per_pitc
#define FILTER_SHIFT    6

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
    int i = MCF5282_PIT3_PCNTR;
    if (MCF5282_PIT3_PCSR & MCF5282_PIT_PCSR_PIF)
        i = MCF5282_PIT3_PCNTR - PITC_PER_TICK;
    return (PITC_PER_TICK - i) * NSEC_PER_PITC;
}

#define Clock_driver_nanoseconds_since_last_tick bsp_clock_nanoseconds_since_last_tick

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()                                       \
    do {                                                                     \
        unsigned idle = IDLE_COUNTER;                                        \
        IDLE_COUNTER = 0;                                                    \
        if (idle > MAX_IDLE_COUNT)                                           \
            MAX_IDLE_COUNT = idle;                                           \
        FILTERED_IDLE = idle + FILTERED_IDLE - (FILTERED_IDLE>>FILTER_SHIFT);\
        MCF5282_PIT3_PCSR |= MCF5282_PIT_PCSR_PIF;                           \
    } while (0)

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new, _old )              \
    do {                                                            \
        _old = (rtems_isr_entry)set_vector(_new, CLOCK_VECTOR, 1);  \
    } while(0)

/*
 * Turn off the clock
 */
#define Clock_driver_support_shutdown_hardware()   \
    do {                                           \
        MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN; \
    } while(0)

/*
 * Set up the clock hardware
 *
 * f_pit = f_clk / 2^(preScaleCode+1) / N  = 1/(us_per_tick/us_per_s)
 *
 * N = f_clk / 2^(preScaleCode+1) * us_per_tick / us_per_s
 *
 * ns_per_pit_clk = ns_per_s / (f_clk / 2^(preScaleCode+1))
 *                = ns_per_s * 2^(preScaleCode+1) / f_clk;
 */
#define Clock_driver_support_initialize_hardware()                       \
    do {                                                                 \
		unsigned long long N;                                            \
        int level;                                                       \
        int preScaleCode = 0;                                            \
		N  = bsp_get_CPU_clock_speed();                                  \
		N *= rtems_configuration_get_microseconds_per_tick();            \
		N /= 2*1000000; /* min_prescale * us_per_s */                    \
		while ( N > 0x10000 ) {                                          \
			preScaleCode++;                                              \
			N >>= 1;                                                     \
		}                                                                \
		PITC_PER_TICK  = N;                                              \
		N  = 2000000000ULL << preScaleCode;                              \
		N /= bsp_get_CPU_clock_speed();                                  \
		NSEC_PER_PITC  = N;                                              \
        IDLE_COUNTER   = 0;                                              \
        FILTERED_IDLE  = 0;                                              \
        MAX_IDLE_COUNT = 0;                                              \
        bsp_allocate_interrupt(PIT3_IRQ_LEVEL, PIT3_IRQ_PRIORITY);       \
        MCF5282_INTC0_ICR58 = MCF5282_INTC_ICR_IL(PIT3_IRQ_LEVEL) |      \
                              MCF5282_INTC_ICR_IP(PIT3_IRQ_PRIORITY);    \
        rtems_interrupt_disable( level );                                \
        MCF5282_INTC0_IMRH &= ~MCF5282_INTC_IMRH_INT58;                  \
        MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN;                       \
        rtems_interrupt_enable( level );                                 \
        MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_OVW |                       \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD;                        \
        MCF5282_PIT3_PMR = PITC_PER_TICK - 1;                            \
        MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD |                       \
                            MCF5282_PIT_PCSR_EN;                         \
    } while (0)

/*
 * Provide our own version of the idle task
 */
Thread bsp_idle_thread(uint32_t ignored)
{
  /* Atomic increment */
  for(;;)
    __asm__ volatile ("addq.l #1,%0"::"m"(IDLE_COUNTER));
}

int rtems_bsp_cpu_load_percentage(void)
{
    return MAX_IDLE_COUNT ?
           (100 - ((100 * (FILTERED_IDLE >> FILTER_SHIFT)) / MAX_IDLE_COUNT)) :
           0;
}

#include "../../../shared/clockdrv_shell.h"
