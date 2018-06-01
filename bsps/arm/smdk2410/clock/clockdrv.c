/*
 *  S3C2400 clock specific using the System Timer
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <s3c24xx.h>

void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_INT_TIMER4,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
#define Clock_driver_support_at_tick()                \
  do {                                                \
        ClearPending(BIT_TIMER4);                     \
  } while(0)


/**
 * Installs the clock ISR. You shouldn't need to change this.
 */
#define Clock_driver_support_install_isr( _new ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)


/**
 * Initialize the hardware for the clock
 *   - Set the frequency
 *   - enable it
 *   - clear any pending interrupts
 *
 * Since you may want the clock always running, you can
 * enable interrupts here. If you do so, the clock_isr_on(),
 * clock_isr_off(), and clock_isr_is_on() functions can be
 * NOPs.
 */
#define Clock_driver_support_initialize_hardware() \
  do { \
        uint32_t cr; \
        uint32_t freq; \
        /* set MUX for Timer4 to 1/16 */ \
        cr=rTCFG1 & 0xFFF0FFFF; \
        rTCFG1=(cr | (3<<16)); \
        freq = get_PCLK(); \
        /* set TIMER4 counter, input freq=PLCK/16/16Mhz*/ \
        freq = (freq /16)/16; \
        rTCNTB4 = ((freq / 1000) * rtems_configuration_get_microseconds_per_tick()) / 1000; \
        /*unmask TIMER4 irq*/ \
        rINTMSK&=~BIT_TIMER4; \
        /* start TIMER4 with autoreload */ \
        cr=rTCON & 0xFF8FFFFF; \
        rTCON=(cr|(0x6<<20)); \
        rTCON=(cr|(0x5<<20)); \
    } while (0)

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
    return;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return 1;
}

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

/* Make sure to include this, and only at the end of the file */
#include "../../../shared/dev/clock/clockimpl.h"
