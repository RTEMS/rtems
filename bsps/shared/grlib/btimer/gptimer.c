/* SPDX-License-Identifier: BSD-2-Clause */

/*  This file contains the driver for the GRLIB GPTIMER timers port. The driver
 *  is implemented by using the tlib.c simple timer layer and the Driver
 *  Manager.
 *
 *  The Driver can be configured using driver resources:
 *
 *  - timerStart  Timer Index if first Timer, this parameters is typically used
 *                in AMP systems for resource allocation. The Timers before
 *                timerStart will not be accessed.
 *  - timerCnt    Number of timers that the driver will use, this parameters is
 *                typically used in AMP systems for resource allocation between
 *                OS instances.
 *  - prescaler   Base prescaler, normally set by bootloader but can be
 *                overridden. The default scaler reload value set by bootloader
 *                is so that Timers operate in 1MHz. Setting the prescaler to a
 *                lower value increase the accuracy of the timers but shortens
 *                the time until underflow happens.
 *  - clockTimer  Used to select a particular timer to be the system clock
 *                timer. This is useful when multiple GPTIMERs cores are
 *                available, or in AMP systems. By default the TLIB selects the
 *                first timer registered as system clock timer.
 *
 *  The BSP define APBUART_INFO_AVAIL in order to add the info routine
 *  used for debugging.
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/gptimer.h>
#include <grlib/gptimer-regs.h>
#include <grlib/io.h>
#include <grlib/tlib.h>

#if defined(LEON3)
#include <bsp/leon3.h>
#endif

#ifdef GPTIMER_INFO_AVAIL
#include <stdio.h>
#endif

#ifdef RTEMS_SMP
#include <rtems/score/processormask.h>
#include <rtems/score/smpimpl.h>
#endif

#include <grlib/grlib_impl.h>

#define DBG(x...)

/* GPTIMER timer private */
struct gptimer_timer_priv {
	struct tlib_dev tdev;	/* Must be first in struct */
	gptimer_timer *tregs;
	char index; /* Timer Index in this driver */
	char tindex; /* Timer Index In Hardware */
	uint32_t irq_ack_mask;
};

/* GPTIMER Core private */
struct gptimer_priv {
	struct drvmgr_dev *dev;
	gptimer *regs;
	unsigned int base_clk;
	unsigned int base_freq;
	unsigned int widthmask;
	char separate_interrupt;
	char isr_installed;

	/* Structure per Timer unit, the core supports up to 8 timers */
	int timer_cnt;
	struct gptimer_timer_priv timers[0];
};

void gptimer_isr(void *data);

#if 0
void gptimer_tlib_irq_register(struct tlib_drv *tdrv, tlib_isr_t func, void *data)
{
	struct gptimer_priv *priv = (struct gptimer_priv *)tdrv;

	if ( SHARED ...)
	
	
	drvmgr_interrupt_register();
}
#endif

/******************* Driver manager interface ***********************/

/* Driver prototypes */
static struct tlib_drv gptimer_tlib_drv;
int gptimer_device_init(struct gptimer_priv *priv);

int gptimer_init1(struct drvmgr_dev *dev);
#ifdef GPTIMER_INFO_AVAIL
static int gptimer_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int, char *argv[]);
#define GTIMER_INFO_FUNC gptimer_info
#else
#define GTIMER_INFO_FUNC NULL
#endif

struct drvmgr_drv_ops gptimer_ops =
{
	.init = {gptimer_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = GTIMER_INFO_FUNC,
};

struct amba_dev_id gptimer_ids[] =
{
	{VENDOR_GAISLER, GAISLER_GPTIMER},
	{VENDOR_GAISLER, GAISLER_GRTIMER},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info gptimer_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GPTIMER_ID,/* Driver ID */
		"GPTIMER_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&gptimer_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&gptimer_ids[0]
};

void gptimer_register_drv (void)
{
	DBG("Registering GPTIMER driver\n");
	drvmgr_drv_register(&gptimer_drv_info.general);
}

int gptimer_init1(struct drvmgr_dev *dev)
{
	struct gptimer_priv *priv;
	gptimer *regs;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	int timer_hw_cnt, timer_cnt, timer_start;
	int i, size;
	struct gptimer_timer_priv *timer;
	union drvmgr_key_value *value;
	uint32_t irq_ack_mask;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	regs = (gptimer *)pnpinfo->apb_slv->start;

	DBG("GPTIMER[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	/* Get number of Timers */
	timer_hw_cnt = GPTIMER_CONFIG_TIMERS_GET(grlib_load_32(&regs->config));

	/* Let user spelect a range of timers to be used. In AMP systems
	 * it is sometimes neccessary to leave timers for other CPU instances.
	 *
	 * The default operation in AMP is to shared the timers within the
	 * first GPTIMER core as below. This can of course be overrided by
	 * driver resources.
	 */
	timer_cnt = timer_hw_cnt;
	timer_start = 0;
#if defined(RTEMS_MULTIPROCESSING) && defined(LEON3)
	if ((dev->minor_drv == 0) && drvmgr_on_rootbus(dev)) {
		timer_cnt = 1;
		timer_start = LEON3_Cpu_Index;
	}
#endif
	value = drvmgr_dev_key_get(dev, "timerStart", DRVMGR_KT_INT);
	if ( value) {
		timer_start = value->i;
		timer_cnt = timer_hw_cnt - timer_start;
	}
	value = drvmgr_dev_key_get(dev, "timerCnt", DRVMGR_KT_INT);
	if ( value && (value->i < timer_cnt) ) {
		timer_cnt = value->i;
	}

	/* Allocate Common Timer Description, size depends on how many timers
	 * are present.
	 */
	size = sizeof(struct gptimer_priv) +
		timer_cnt*sizeof(struct gptimer_timer_priv);
	priv = dev->priv = grlib_calloc(1, size);
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;
	priv->regs = regs;

	/* The Base Frequency of the GPTIMER core is the same as the
	 * frequency of the AMBA bus it is situated on.
	 */
	drvmgr_freq_get(dev, DEV_APB_SLV, &priv->base_clk);

	/* This core will may provide important Timer functionality
	 * to other drivers and the RTEMS kernel, the Clock driver
	 * may for example use this device. So the Timer driver must be
	 * initialized in the first iiitialization stage.
	 */

	/*** Initialize Hardware ***/

	/* If user request to set prescaler, we will do that. However, note
	 * that doing so for the Root-Bus GPTIMER may affect the RTEMS Clock
	 * so that Clock frequency is wrong.
	 */
	value = drvmgr_dev_key_get(priv->dev, "prescaler", DRVMGR_KT_INT);
	if ( value )
		grlib_store_32(&regs->sreload, value->i);

	/* Get Frequency that the timers are operating in (after prescaler) */
	priv->base_freq = priv->base_clk / (grlib_load_32(&regs->sreload) + 1);

	/* Stop Timer and probe Pending bit. In newer hardware the
	 * timer has pending bit is cleared by writing a one to it,
	 * whereas older versions it is cleared with a zero.
	 */
	grlib_store_32(&regs->timer[timer_start].tctrl, GPTIMER_TCTRL_IP);
	if ((grlib_load_32(&regs->timer[timer_start].tctrl) & GPTIMER_TCTRL_IP) != 0)
		irq_ack_mask = ~GPTIMER_TCTRL_IP;
	else
		irq_ack_mask = ~0U;

	/* Probe timer register width mask */
	grlib_store_32(&regs->timer[timer_start].tcntval, 0xffffffff);
	priv->widthmask = grlib_load_32(&regs->timer[timer_start].tcntval);

	priv->timer_cnt = timer_cnt;
	for (i=0; i<timer_cnt; i++) {
		timer = &priv->timers[i];
		timer->index = i;
		timer->tindex = i + timer_start;
		timer->tregs = &regs->timer[(int)timer->tindex];
		timer->tdev.drv = &gptimer_tlib_drv;
		timer->irq_ack_mask = irq_ack_mask;

		/* Register Timer at Timer Library */
		tlib_dev_reg(&timer->tdev);
	}

	/* Check Interrupt support implementation, two cases:
	 *  A. All Timers share one IRQ
	 *  B. Each Timer have an individual IRQ. The number is:
	 *        BASE_IRQ + timer_index
	 */
	priv->separate_interrupt = (grlib_load_32(&regs->config) & GPTIMER_CONFIG_SI) != 0;

	return DRVMGR_OK;
}

#ifdef GPTIMER_INFO_AVAIL
static int gptimer_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int argc, char *argv[])
{
	struct gptimer_priv *priv = dev->priv;
	struct gptimer_timer_priv *timer;
	char buf[64];
	int i;

	if (priv == NULL || argc != 0)
		return -DRVMGR_EINVAL;

	sprintf(buf, "Timer Count: %d", priv->timer_cnt);
	print_line(p, buf);
	sprintf(buf, "REGS:        0x%08x", (unsigned int)priv->regs);
	print_line(p, buf);
	sprintf(buf, "BASE SCALER: %d", grlib_load_32(&priv->regs->sreload));
	print_line(p, buf);
	sprintf(buf, "BASE FREQ:   %dkHz", priv->base_freq / 1000);
	print_line(p, buf);
	sprintf(buf, "SeparateIRQ: %s", priv->separate_interrupt ? "YES":"NO");
	print_line(p, buf);

	for (i=0; i<priv->timer_cnt; i++) {
		timer = &priv->timers[i];
		sprintf(buf, " - TIMER HW Index %d -", timer->tindex);
		print_line(p, buf);
		sprintf(buf, " TLIB Index: %d", timer->index);
		print_line(p, buf);
		sprintf(buf, " RELOAD REG: %d", grlib_load_32(&timer->tregs->trldval));
		print_line(p, buf);
		sprintf(buf, " CTRL REG:   %d", grlib_load_32(&timer->tregs->tctrl));
		print_line(p, buf);
	}

	return DRVMGR_OK;
}
#endif

static inline struct gptimer_priv *priv_from_timer(struct gptimer_timer_priv *t)
{
	return (struct gptimer_priv *)
		((unsigned int)t -
		sizeof(struct gptimer_priv) -
		t->index * sizeof(struct gptimer_timer_priv));
}

static int gptimer_tlib_int_pend(struct tlib_dev *hand, int ack)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	uint32_t tctrl;

	tctrl = grlib_load_32(&timer->tregs->tctrl);

	if ((tctrl & (GPTIMER_TCTRL_IP | GPTIMER_TCTRL_IE)) ==
		(GPTIMER_TCTRL_IP | GPTIMER_TCTRL_IE)) {
		/* clear Pending IRQ ? */
		if (ack) {
			tctrl &= timer->irq_ack_mask;
			grlib_store_32(&timer->tregs->tctrl, tctrl);
		}
		return 1; /* timer generated IRQ */
	} else
		return 0; /* was not timer causing IRQ */
}

void gptimer_isr(void *data)
{
	struct gptimer_priv *priv = data;
	int i;

	/* Check all timers for IRQ */
	for (i=0;i<priv->timer_cnt; i++) {
		if (gptimer_tlib_int_pend((void *)&priv->timers[i], 0)) {
			/* IRQ Was generated by Timer and Pending flag has *not*
			 * yet been cleared, this is to allow ISR to look at
			 * pending bit. Call ISR registered. Clear pending bit.
			 */
			if (priv->timers[i].tdev.isr_func) {
				priv->timers[i].tdev.isr_func(
					priv->timers[i].tdev.isr_data);
			}
			gptimer_tlib_int_pend((void *)&priv->timers[i], 1);
		}
	}
}

static void gptimer_tlib_reset(struct tlib_dev *hand)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	uint32_t tctrl;

	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl &= timer->irq_ack_mask;
	tctrl &= GPTIMER_TCTRL_IP;
	grlib_store_32(&timer->tregs->tctrl, tctrl);
	grlib_store_32(&timer->tregs->trldval, 0xffffffff);
	grlib_store_32(&timer->tregs->tctrl, GPTIMER_TCTRL_LD);
}

static void gptimer_tlib_get_freq(
	struct tlib_dev *hand,
	unsigned int *basefreq,
	unsigned int *tickrate)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);

	/* Calculate base frequency from Timer Clock and Prescaler */
	if ( basefreq )
		*basefreq = priv->base_freq;
	if ( tickrate )
		*tickrate = grlib_load_32(&timer->tregs->trldval) + 1;
}

static int gptimer_tlib_set_freq(struct tlib_dev *hand, unsigned int tickrate)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;

	grlib_store_32(&timer->tregs->trldval, tickrate - 1);

	/*Check that value was allowed (Timer may not be as wide as expected)*/
	if (grlib_load_32(&timer->tregs->trldval) != (tickrate - 1))
		return -1;
	else
		return 0;
}

static void gptimer_tlib_irq_reg(struct tlib_dev *hand, tlib_isr_t func, void *data, int flags)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);
	uint32_t tctrl;

	if ( priv->separate_interrupt ) {
		drvmgr_interrupt_register(priv->dev, timer->tindex,
						"gptimer", func, data);
	} else {
		if (priv->isr_installed == 0) {
			/* Shared IRQ handler */
			drvmgr_interrupt_register(
				priv->dev,
				0,
				"gptimer_shared",
				gptimer_isr,
				priv);
		}
		priv->isr_installed++;
	}

#if RTEMS_SMP
	if (flags & TLIB_FLAGS_BROADCAST) {
		int tindex = 0;

		if (priv->separate_interrupt) {
			/* Offset interrupt number with HW subtimer index */
			tindex = timer->tindex;
		}
		drvmgr_interrupt_set_affinity(priv->dev, tindex,
					      _SMP_Get_online_processors());
	}
#endif

	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl |= GPTIMER_TCTRL_IE;
	grlib_store_32(&timer->tregs->tctrl, tctrl);
}

static void gptimer_tlib_irq_unreg(struct tlib_dev *hand, tlib_isr_t func, void *data)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);
	uint32_t tctrl;

	/* Turn off IRQ at source, unregister IRQ handler */
	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl &= ~GPTIMER_TCTRL_IE;
	grlib_store_32(&timer->tregs->tctrl, tctrl);

	if ( priv->separate_interrupt ) {
		drvmgr_interrupt_unregister(priv->dev, timer->tindex,
						func, data);
	} else {
		timer->tdev.isr_func = NULL;
		priv->isr_installed--;
		if (priv->isr_installed == 0) {
			drvmgr_interrupt_unregister(priv->dev, 0,
							gptimer_isr, priv);
		}
	}
}

static void gptimer_tlib_start(struct tlib_dev *hand, int once)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	uint32_t tctrl;

	/* Load the selected frequency before starting Frequency */
	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl &= timer->irq_ack_mask;
	tctrl &= ~GPTIMER_TCTRL_RS;
	tctrl |= GPTIMER_TCTRL_LD | GPTIMER_TCTRL_EN;
	if ( once == 0 )
		tctrl |= GPTIMER_TCTRL_RS; /* Restart Timer */
	grlib_store_32(&timer->tregs->tctrl, tctrl);
}

static void gptimer_tlib_stop(struct tlib_dev *hand)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	uint32_t tctrl;

	/* Load the selected Frequency */
	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl &= ~(GPTIMER_TCTRL_EN|GPTIMER_TCTRL_IP);
	grlib_store_32(&timer->tregs->tctrl, tctrl);
}

static void gptimer_tlib_restart(struct tlib_dev *hand)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	uint32_t tctrl;

	tctrl = grlib_load_32(&timer->tregs->tctrl);
	tctrl |= GPTIMER_TCTRL_LD | GPTIMER_TCTRL_EN;
	grlib_store_32(&timer->tregs->tctrl, tctrl);
}

static void gptimer_tlib_get_counter(
	struct tlib_dev *hand,
	unsigned int *counter)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;

	*counter = grlib_load_32(&timer->tregs->tcntval);
}

static void gptimer_tlib_get_widthmask(
	struct tlib_dev *hand,
	unsigned int *widthmask)
{
	struct gptimer_timer_priv *timer = (struct gptimer_timer_priv *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);

	*widthmask = priv->widthmask;
}

static struct tlib_drv gptimer_tlib_drv =
{
	.reset = gptimer_tlib_reset,
	.get_freq = gptimer_tlib_get_freq,
	.set_freq = gptimer_tlib_set_freq,
	.irq_reg = gptimer_tlib_irq_reg,
	.irq_unreg = gptimer_tlib_irq_unreg,
	.start = gptimer_tlib_start,
	.stop = gptimer_tlib_stop,
	.restart = gptimer_tlib_restart,
	.get_counter = gptimer_tlib_get_counter,
	.custom = NULL,
	.int_pend = gptimer_tlib_int_pend,
	.get_widthmask = gptimer_tlib_get_widthmask,
};
