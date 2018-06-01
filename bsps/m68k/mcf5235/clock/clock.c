/*
 * Use the last periodic interval timer (PIT3) as the system clock.
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5235/mcf5235.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+39)

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()             \
    do {                                           \
        MCF5235_PIT_PCSR3 |= MCF5235_PIT_PCSR_PIF; \
    } while (0)                                    \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
    set_vector(_new, CLOCK_VECTOR, 1)

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every 10,000 microseconds
 * so we need to set prescaler to 64 and the PMR register to 0x2DC6
 */
#define Clock_driver_support_initialize_hardware()                       \
    do {                                                                 \
        int level;                                                       \
        int preScaleCode = 6;                                            \
        MCF5235_INTC0_ICR39 = MCF5235_INTC_ICR_IL(PIT3_IRQ_LEVEL) |      \
                              MCF5235_INTC_ICR_IP(PIT3_IRQ_PRIORITY);    \
        rtems_interrupt_disable( level );                                \
        MCF5235_INTC0_IMRH &= ~MCF5235_INTC0_IMRH_INT39;                  \
	MCF5235_PIT_PCSR3 &= ~MCF5235_PIT_PCSR_EN;               \
        rtems_interrupt_enable( level );                                 \
		MCF5235_PIT_PMR3 = 0x2DC6;                               \
		MCF5235_PIT_PCSR3 = MCF5235_PIT_PCSR_PRE(preScaleCode) | \
                            MCF5235_PIT_PCSR_PIE |                       \
                            MCF5235_PIT_PCSR_RLD |                       \
                            MCF5235_PIT_PCSR_EN;                         \
    } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
