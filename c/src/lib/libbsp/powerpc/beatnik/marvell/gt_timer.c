/* Driver for discovery timers and watchdog */

/* 
 * Acknowledgements:
 * Valuable information was obtained from the following drivers
 *   netbsd: (C) Allegro Networks Inc; Wasabi Systems Inc.
 *   linux:  (C) MontaVista, Software, Inc; Mark A. Greer.
 *   rtems:  (C) Brookhaven National Laboratory; K. Feng
 * but this implementation is original work by the author.
 */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 
#include <rtems.h>
#include <bsp/gtreg.h>
#include <libcpu/io.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>

#include <stdint.h>

#include "gt_timer.h"

#define DEBUG

static inline uint32_t gt_rd(uint32_t off)
{
		return in_le32( (volatile unsigned *)(BSP_MV64x60_BASE+off) );
}

static inline void gt_wr(uint32_t off, uint32_t val)
{
		out_le32( (volatile unsigned *)(BSP_MV64x60_BASE+off), val);
}

static inline uint32_t gt_timer_bitmod(uint32_t off, uint32_t clr, uint32_t set)
{
	unsigned	flags;
	uint32_t	rval;
		rtems_interrupt_disable(flags);
			rval = gt_rd( off );
			gt_wr( off, (rval & ~clr) | set );
		rtems_interrupt_enable(flags);
	return rval;
}

#define GT_TIMER_MAX			3
#define TIMER_ARGCHECK(t)	do { if ((t)<0 || (t)>GT_TIMER_MAX) return -1; } while (0)

static struct {
	void (*isr)(void *);
	void  *arg;
} gt_timer_isrs[GT_TIMER_MAX+1] = {{0},};

uint32_t BSP_timer_read(uint32_t timer)
{
	TIMER_ARGCHECK(timer);
	return gt_rd(GT_TIMER_0 + (timer<<2));
}

int
BSP_timer_start(uint32_t timer, uint32_t period)
{
	TIMER_ARGCHECK(timer);
	gt_wr(GT_TIMER_0 + (timer<<2), period);
	return 0;
}

int
BSP_timer_stop(uint32_t timer)
{
	TIMER_ARGCHECK(timer);
	/* disable, clear period, re-enable */
	gt_timer_bitmod(GT_TIMER_0_3_Ctl, GT_TIMER_0_Ctl_Enb << (timer<<3), 0);
	gt_wr(GT_TIMER_0 + (timer<<2), 0);
	gt_timer_bitmod(GT_TIMER_0_3_Ctl, 0, GT_TIMER_0_Ctl_Enb << (timer<<3));
	return 0;
}

int
BSP_timer_setup(uint32_t timer, void (*isr)(void *arg), void *arg, int reload)
{
	TIMER_ARGCHECK(timer);
	if ( isr && gt_timer_isrs[timer].isr )
		return -1;
	BSP_timer_stop(timer);
	/* mask and clear */
	gt_timer_bitmod(GT_TIMER_0_3_Intr_Msk, GT_TIMER_0_Intr<<timer, 0);
	gt_timer_bitmod(GT_TIMER_0_3_Intr_Cse, GT_TIMER_0_Intr<<timer, 0);

	/* set reload bit */
	if ( reload )
		gt_timer_bitmod(GT_TIMER_0_3_Ctl, 0, GT_TIMER_0_Ctl_Rld << (timer<<3));
	else
		gt_timer_bitmod(GT_TIMER_0_3_Ctl, GT_TIMER_0_Ctl_Rld << (timer<<3), 0);

	asm volatile("":::"memory");

	if ( isr ) {
		gt_timer_isrs[timer].isr = isr;
		gt_timer_isrs[timer].arg = arg;
		asm volatile("":::"memory");
		gt_timer_bitmod(GT_TIMER_0_3_Intr_Msk, 0, GT_TIMER_0_Intr<<timer);
	} else {
		gt_timer_isrs[timer].isr = 0;
		gt_timer_isrs[timer].arg = 0;
	}
	return 0;
}

static void
gt_timer_hdl(rtems_irq_hdl_param arg)
{
int	  iarg = (int)arg;
int       timer;
uint32_t  bit;

	for ( ; iarg; iarg >>= 4 ) {
		timer = (iarg & 0xf)-1;
		bit   = GT_TIMER_0_Intr<<timer;
		if ( gt_timer_bitmod(GT_TIMER_0_3_Intr_Cse, bit, 0) & bit ) {
			/* cause was set */
			if ( ! gt_timer_isrs[timer].isr ) {
				printk("gt_timer: warning; no ISR connected but and IRQ happened (timer # %i)\n", timer);
				/* mask */
				gt_timer_bitmod(GT_TIMER_0_3_Intr_Msk, bit, 0);
			} else {
				gt_timer_isrs[timer].isr(gt_timer_isrs[timer].arg);
			}
		}
	}
}

int
BSP_timers_initialize(void)
{
rtems_irq_connect_data xx = {0};
int                    i, ainc, arg;

	xx.hdl    = gt_timer_hdl;
	xx.on     = 0;
	xx.off    = 0;
	xx.isOn   = 0;

	switch (BSP_getDiscoveryVersion(0)) {
		case MV_64360:
			i    = 3;
			ainc = 1;
			arg  = 4;
			break;
		default:
			i    = 1;
			ainc = 0x0202;
			arg  = 0x0403;
			break;
	}

	for ( ; i>=0; i--, arg-=ainc ) {
		xx.name   = BSP_IRQ_TIME0_1 + i;
		xx.handle = (rtems_irq_hdl_param)arg;
		if ( !BSP_install_rtems_irq_handler(&xx) )
			return -1;
	}

	return 0;
}

int
BSP_timers_uninstall(void)
{
rtems_irq_connect_data xx = {0};
int                    i;

	xx.hdl    = gt_timer_hdl;
	xx.on     = 0;
	xx.off    = 0;
	xx.isOn   = 0;

	for ( i=0; i<= GT_TIMER_MAX; i++ ) {
		if ( BSP_timer_setup(i, 0, 0, 0) )
			return -1;
	}

	switch (BSP_getDiscoveryVersion(0)) {
		case MV_64360:
			i = 3;
			break;
		default:
			i = 1;
			break;
	}

	for ( ; i >= 0; i-- ) {
		xx.name  = BSP_IRQ_TIME0_1 + i;
		BSP_get_current_rtems_irq_handler(&xx);
		if ( !BSP_remove_rtems_irq_handler(&xx) )
			return -1;
	}

	return 0;
}

uint32_t
BSP_timer_clock_get(uint32_t timer)
{
	return BSP_bus_frequency;
}

int BSP_timer_instances(void)
{
	return GT_TIMER_MAX + 1;
}

#ifdef DEBUG
void BSP_timer_test_isr(void *arg)
{
	printk("TIMER IRQ (user arg 0x%x)\n",arg);
}
#endif

/* On a 64260A we can't read the status (on/off), apparently
 * so we maintain it locally and assume the firmware has
 * not enabled the dog initially...
 */
static uint32_t wdog_on = 0x00ffffff;

static uint32_t rd_wdcnf(void)
{
	uint32_t cnf = gt_rd(GT_WDOG_Config);
	/* BSD driver says that on the 64260A we always
	 * read 0xffffffff so we have to maintain the
	 * status locally (and hope we get the initial
	 * value right).
	 */
	if ( ~0 == cnf )
		cnf = wdog_on;
	return cnf;
}

/* change on/off state assume caller has IRQs disabled */
static void dog_toggle(uint32_t ctl)
{
	ctl &= ~( GT_WDOG_Config_Ctl1a | GT_WDOG_Config_Ctl1b \
	        | GT_WDOG_Config_Ctl2a | GT_WDOG_Config_Ctl2b);
	gt_wr(GT_WDOG_Config, ctl | GT_WDOG_Config_Ctl1a);
	gt_wr(GT_WDOG_Config, ctl | GT_WDOG_Config_Ctl1b);
}

static void dog_pet(uint32_t ctl)
{
	ctl &= ~( GT_WDOG_Config_Ctl1a | GT_WDOG_Config_Ctl1b \
	        | GT_WDOG_Config_Ctl2a | GT_WDOG_Config_Ctl2b);
	gt_wr(GT_WDOG_Config, ctl | GT_WDOG_Config_Ctl2a);
	gt_wr(GT_WDOG_Config, ctl | GT_WDOG_Config_Ctl2b);
}


/* Enable watchdog and set a timeout (in us)
 * a timeout of 0xffffffff selects the old/existing
 * timeout.
 *
 * RETURNS 0 on success
 */
int
BSP_watchdog_enable(uint32_t timeout_us)
{
unsigned long long x = timeout_us;
unsigned flags;
uint32_t ctl;

	x *= BSP_bus_frequency;
	x /= 256;     /* there seems to be a prescaler */
    x /= 1000000; /* us/s                          */

	if ( x > (1<<24)-1 )
		x = (1<<24)-1;

	if ( 0xffffffff != timeout_us )
		timeout_us = x;

	rtems_interrupt_disable(flags);

	ctl = rd_wdcnf();

	/* if enabled, disable first */
	if ( GT_WDOG_Config_Enb & ctl ) {
		dog_toggle(ctl);
	}
	if ( 0xffffffff == timeout_us ) {
		timeout_us = ctl & ((1<<24)-1);
		dog_toggle(ctl);
		dog_pet(ctl);
	} else {
		gt_wr(GT_WDOG_Config, timeout_us | GT_WDOG_Config_Ctl1a);
		gt_wr(GT_WDOG_Config, timeout_us | GT_WDOG_Config_Ctl1b);
	}

	wdog_on = GT_WDOG_Config_Enb | timeout_us;

	rtems_interrupt_enable(flags);
	return 0;
}

/* Disable watchdog
 * RETURNS 0 on success
 */
int BSP_watchdog_disable(void)
{
unsigned long flags;
uint32_t      ctl;

	rtems_interrupt_disable(flags);

	ctl = rd_wdcnf();

	if ( (GT_WDOG_Config_Enb & ctl) ) {
		dog_toggle(ctl);
		wdog_on = ctl & ~(GT_WDOG_Config_Enb);
	}

	rtems_interrupt_enable(flags);
	return 0;
}

/* Check status -- unfortunately there seems to be no way
 * to read the running value...
 *
 * RETURNS nonzero if enabled/running, zero if disabled/stopped
 */
int BSP_watchdog_status(void)
{
uint32_t ctl = rd_wdcnf();

	/* report also the current period */
	return GT_WDOG_Config_Enb & ctl ? ctl : 0;
}

/* Pet the watchdog (rearm to configured timeout)
 * RETURNS: 0 on success, nonzero on failure (watchdog
 * currently not running).
 */
int BSP_watchdog_pet(void)
{
unsigned long flags;
	if ( !wdog_on )
		return -1;
	rtems_interrupt_disable(flags);
		dog_pet(rd_wdcnf());
	rtems_interrupt_enable(flags);
	return 0;
}


#ifdef DEBUG_MODULAR
int
_cexpModuleFinalize(void *unused)
{
	BSP_watchdog_disable();
	return BSP_timers_uninstall();
}

void
_cexpModuleInitialize(void *unused)
{
	BSP_timers_initialize();
}
#endif
