/*
 * This routine initializes the MC68360 Periodic Interval Timer
 *
 * The PIT has rather poor resolution, but it is easy to set up
 * and requires no housekeeping once it is going.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/m68k/m68360.h>

#define CLOCK_VECTOR     120
#define CLOCK_IRQ_LEVEL    4

char M360DefaultWatchdogFeeder = 1;

/*
 * RTEMS and hardware have different notions of clock rate.
 */
static unsigned long rtems_nsec_per_tick;
static unsigned long pit_nsec_per_tick;
static unsigned long nsec;

/*
 * Periodic interval timer interrupt handler
 *    See if it's really time for a `tick'
 *    Perform a dummy read of DPRAM (work around bug in Rev. B of the 68360).
 *    Feed the watchdog
 *        Application code can override this by
 *        setting M360DefaultWatchdogFeeder to zero.
 */
#define Clock_driver_support_at_tick()  \
    do {                                   \
        nsec += pit_nsec_per_tick;         \
        if (nsec >= rtems_nsec_per_tick)   \
            return;                        \
        nsec -= rtems_nsec_per_tick;       \
        m360.dpram0[0];                    \
        if (M360DefaultWatchdogFeeder) {   \
            m360.swsr = 0x55;              \
            m360.swsr = 0xAA;              \
        }                                  \
    } while (0)                            \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new, _old ) \
    do {                                                          \
        _old = (rtems_isr_entry)set_vector(_new, CLOCK_VECTOR, 1);  \
    } while(0)

/*
 * Turn off the clock
 */
#define Clock_driver_support_shutdown_hardware() \
    do {                       \
        m360.pitr &= ~0xFF;    \
    } while(0)

/*
 * Set up the clock hardware
 *     The rate at which the periodic interval timer
 *     can generate interrupts is almost certainly not
 *     the same as desired by the BSP configuration.
 *     Handle the difference by choosing the largest PIT
 *     interval which is less than or equal to the RTEMS
 *     interval and skipping some hardware interrupts.
 *     To reduce the jitter in the calls to RTEMS the
 *     hardware interrupt interval is never greater than
 *     the maximum non-prescaled value from the PIT.
 *
 *     For a 25 MHz external clock the basic clock rate is
 *        40 nsec * 128 * 4 = 20.48 usec/tick
 */
extern int m360_clock_rate;

#define Clock_driver_support_initialize_hardware() \
    do {                                                                      \
        unsigned int divisor;                                                 \
        unsigned long nsec_per_chip_tick = 1000000000 / m360_clock_rate;      \
        unsigned long nsec_per_pit_tick = 512 * nsec_per_chip_tick;           \
        rtems_nsec_per_tick = rtems_configuration_get_microseconds_per_tick() * 1000; \
        divisor = rtems_nsec_per_tick / nsec_per_pit_tick;                    \
        if (divisor > 255)                                                    \
            divisor = 255;                                                    \
        else if (divisor == 0)                                                \
            divisor = 1;                                                      \
        pit_nsec_per_tick = nsec_per_pit_tick * divisor;                      \
        m360.pitr &= ~0x1FF;                                                  \
        m360.picr = (CLOCK_IRQ_LEVEL << 8) | CLOCK_VECTOR;                    \
        m360.pitr |= divisor;                                                 \
    } while (0)

#include "../../../shared/clockdrv_shell.h"
