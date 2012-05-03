/*
 * Test nested interrupts.
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 2007
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


/*
 * Needs board with 2 available openpic timers
 *
 * 'timer_instdis(timer, install, period)'
 *
 * installs 'timer_isr' to openpic timer # 'timer'.
 * The interrupt priority is set to 8 + timer#
 *
 * The timer_isr prints a message then polls
 * the variable 'timer_poll' while it has the value
 * of the timer # then sets it to -1 and prints
 * the 'leave' message.
 *
 * To test nested interrupts:
 *
 *  timer_instdis(0, 1, period)
 *  wait_a_bit()
 *  timer_instdis(1, 1, period)
 *  timer_poll = 0;
 *
 *  As soon as timer 0's IRQ fires the
 *  isr prints
 *     TIMER ISR (0) ...
 *  then starts polling (since timer_poll == 0 )
 *  eventually, timer 1 goes off, interrupts (because
 *  it's priority is 9 (i.e., higher than timer 0's priority)
 *  and prints
 *     TIMER ISR (1)
 *  it skips polling since timer_poll is 0, not 1 but
 *  resets timer_poll -1 and prints
 *     Leaving ISR (1)
 *  timer 0 isr resumes polling and finds timer_poll == -1
 *  so it also writes -1 to timer_poll and exits, printing
 *     Leaving ISR (0)
 *
 *  The timer IRQs can be unhooked with
 *     timer_instdis( 0, 0, period );
 *     timer_instdis( 1, 0, period );
 */
#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <inttypes.h>
#include <stdio.h>

volatile int timer_poll=-1;

static void timer_isr(rtems_irq_hdl_param p)
{
uint32_t top;
uint32_t r1;
uint32_t lat = (OpenPIC->Global.Timer[(int)p].Current_Count & 0x7fffffff);

	lat = OpenPIC->Global.Timer[(int)p].Base_Count - lat;

	asm volatile("mfspr %0, %2; mr %1, 1":"=r"(top),"=r"(r1):"i"(SPRG1));
	printk("Timer ISR (%i): LAT: 0x%08x, TOP 0x%08x, BOT 0x%08x, SP 0x%08x\n",
		(int)p, lat, top, top-rtems_configuration_get_interrupt_stack_size(), r1);
	printk("_ISR_Nest_level %i\n", _ISR_Nest_level);
	while ( timer_poll == (int)p )
		;
	timer_poll = -1;

	printk("Leaving ISR (%i)\n",(int)p);
}

int timer_instdis(int t, int inst, unsigned period)
{
rtems_irq_connect_data xx;
	xx.name   = BSP_MISC_IRQ_LOWEST_OFFSET + t;
	xx.hdl    = timer_isr;
	xx.handle = (rtems_irq_hdl_param)t;
	xx.on     = 0;
	xx.off    = 0;
	xx.isOn   = 0;
	if ( !inst ) {
		openpic_maptimer(t, 0);
		openpic_inittimer(t, 0, 0);
	}
	if ( ! ( inst ? BSP_install_rtems_irq_handler(&xx) : BSP_remove_rtems_irq_handler(&xx) ) ) {
		openpic_maptimer(t, 0);
		openpic_inittimer(t, 0, 0);
		fprintf(stderr,"unable to %s timer ISR #%i\n", inst ? "install" : "remove", t);
		return -1;
	}
	if ( inst ) {
		openpic_maptimer( t, 1 );
		openpic_inittimer( t, 8 + t, OPENPIC_VEC_SOURCE - BSP_PCI_IRQ_LOWEST_OFFSET + xx.name );
		openpic_settimer( t, period, 1 );
	}
	return 0;
}

