/*
 * Use the last periodic interval timer (PIT3) as the system clock.
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5282/mcf5282.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+58)

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()             \
    do {                                           \
        MCF5282_PIT3_PCSR |= MCF5282_PIT_PCSR_PIF; \
    } while (0)                                    \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new, _old )             \
    do {                                                           \
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
 * We need to have 1 interrupt every 10,000 microseconds
 * so we need to set prescaler to 64 and the PMR register to 0x23FE
 */
#define Clock_driver_support_initialize_hardware()                       \
    do {                                                                 \
        int level;                                                       \
        int preScaleCode = 5;                                             \
        MCF5282_INTC0_ICR58 = MCF5282_INTC_ICR_IL(PIT3_IRQ_LEVEL) |      \
                              MCF5282_INTC_ICR_IP(PIT3_IRQ_PRIORITY);    \
        rtems_interrupt_disable( level );                                \
        MCF5282_INTC0_IMRH &= ~MCF5282_INTC_IMRH_INT58;                  \
		MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN;                       \
        rtems_interrupt_enable( level );                                 \
		MCF5282_PIT3_PMR = 0x23FE;  \
		MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD |                       \
                            MCF5282_PIT_PCSR_EN;                         \
    } while (0)

#include "../../../shared/clockdrv_shell.h"
