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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <stdlib.h>
#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <grlib.h>
#include <bsp/gptimer.h>
#include <bsp/tlib.h>

#if defined(LEON3)
#include <leon.h>
#endif

#ifdef GPTIMER_INFO_AVAIL
#include <stdio.h>
#endif

/* GPTIMER Core Configuration Register (READ-ONLY) */
#define GPTIMER_CFG_TIMERS_BIT	0
#define GPTIMER_CFG_IRQ_BIT	3
#define GPTIMER_CFG_SI_BIT	8
#define GPTIMER_CFG_DF_BIT	9

#define GPTIMER_CFG_TIMERS	(0x7<<GPTIMER_CFG_TIMERS_BIT)
#define GPTIMER_CFG_IRQ		(0x1f<<GPTIMER_CFG_IRQ_BIT)
#define GPTIMER_CFG_SI		(1<<GPTIMER_CFG_SI_BIT)
#define GPTIMER_CFG_DF		(1<<GPTIMER_CFG_DF_BIT)

/* GPTIMER Timer Control Register */
#define GPTIMER_CTRL_EN_BIT	0
#define GPTIMER_CTRL_RS_BIT	1
#define GPTIMER_CTRL_LD_BIT	2
#define GPTIMER_CTRL_IE_BIT	3
#define GPTIMER_CTRL_IP_BIT	4
#define GPTIMER_CTRL_CH_BIT	5
#define GPTIMER_CTRL_DH_BIT	6

#define GPTIMER_CTRL_EN	(1<<GPTIMER_CTRL_EN_BIT)
#define GPTIMER_CTRL_RS	(1<<GPTIMER_CTRL_RS_BIT)
#define GPTIMER_CTRL_LD	(1<<GPTIMER_CTRL_LD_BIT)
#define GPTIMER_CTRL_IE	(1<<GPTIMER_CTRL_IE_BIT)
#define GPTIMER_CTRL_IP	(1<<GPTIMER_CTRL_IP_BIT)
#define GPTIMER_CTRL_CH	(1<<GPTIMER_CTRL_CH_BIT)
#define GPTIMER_CTRL_DH	(1<<GPTIMER_CTRL_DH_BIT)

#define DBG(x...)

/* GPTIMER timer private */
struct gptimer_timer {
	struct tlib_dev tdev;	/* Must be first in struct */
	struct gptimer_timer_regs *tregs;
	char index; /* Timer Index in this driver */
	char tindex; /* Timer Index In Hardware */
	unsigned char irq_ack_mask;
};

/* GPTIMER Core private */
struct gptimer_priv {
	struct drvmgr_dev *dev;
	struct gptimer_regs *regs;
	unsigned int base_clk;
	unsigned int base_freq;
	char separate_interrupt;
	char isr_installed;

	/* Structure per Timer unit, the core supports up to 8 timers */
	int timer_cnt;
	struct gptimer_timer timers[0];
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
	struct gptimer_regs *regs;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	int timer_hw_cnt, timer_cnt, timer_start;
	int i, size;
	struct gptimer_timer *timer;
	union drvmgr_key_value *value;
	unsigned char irq_ack_mask;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	regs = (struct gptimer_regs *)pnpinfo->apb_slv->start;

	DBG("GPTIMER[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	/* Get number of Timers */
	timer_hw_cnt = regs->cfg & GPTIMER_CFG_TIMERS;

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
		timer_cnt*sizeof(struct gptimer_timer);
	priv = dev->priv = (struct gptimer_priv *)malloc(size);
	if ( !priv )
		return DRVMGR_NOMEM;
	memset(priv, 0, size);
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
		regs->scaler_reload = value->i;

	/* Get Frequency that the timers are operating in (after prescaler) */
	priv->base_freq = priv->base_clk / (priv->regs->scaler_reload + 1);

	/* Stop Timer and probe Pending bit. In newer hardware the
	 * timer has pending bit is cleared by writing a one to it,
	 * whereas older versions it is cleared with a zero.
	 */
	priv->regs->timer[timer_start].ctrl = GPTIMER_CTRL_IP;
	if ((priv->regs->timer[timer_start].ctrl & GPTIMER_CTRL_IP) != 0)
		irq_ack_mask = ~GPTIMER_CTRL_IP;
	else
		irq_ack_mask = ~0;

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
	priv->separate_interrupt = regs->cfg & GPTIMER_CFG_SI;

	return DRVMGR_OK;
}

#ifdef GPTIMER_INFO_AVAIL
static int gptimer_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int argc, char *argv[])
{
	struct gptimer_priv *priv = dev->priv;
	struct gptimer_timer *timer;
	char buf[64];
	int i;

	if (priv == NULL || argc != 0)
		return -DRVMGR_EINVAL;

	sprintf(buf, "Timer Count: %d", priv->timer_cnt);
	print_line(p, buf);
	sprintf(buf, "REGS:        0x%08x", (unsigned int)priv->regs);
	print_line(p, buf);
	sprintf(buf, "BASE SCALER: %d", priv->regs->scaler_reload);
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
		sprintf(buf, " RELOAD REG: %d", timer->tregs->reload);
		print_line(p, buf);
		sprintf(buf, " CTRL REG:   %d", timer->tregs->ctrl);
		print_line(p, buf);
	}

	return DRVMGR_OK;
}
#endif

static inline struct gptimer_priv *priv_from_timer(struct gptimer_timer *t)
{
	return (struct gptimer_priv *)
		((unsigned int)t -
		sizeof(struct gptimer_priv) -
		t->index * sizeof(struct gptimer_timer));
}

static int gptimer_tlib_int_pend(struct tlib_dev *hand, int ack)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;
	unsigned int ctrl = timer->tregs->ctrl;

	if ((ctrl & (GPTIMER_CTRL_IP | GPTIMER_CTRL_IE)) ==
		(GPTIMER_CTRL_IP | GPTIMER_CTRL_IE)) {
		/* clear Pending IRQ ? */
		if (ack)
			timer->tregs->ctrl = ctrl & timer->irq_ack_mask;
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
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;

	timer->tregs->ctrl = (timer->tregs->ctrl & timer->irq_ack_mask) &
			     GPTIMER_CTRL_IP;
	timer->tregs->reload = 0xffffffff;
	timer->tregs->ctrl = GPTIMER_CTRL_LD;
}

static void gptimer_tlib_get_freq(
	struct tlib_dev *hand,
	unsigned int *basefreq,
	unsigned int *tickrate)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);

	/* Calculate base frequency from Timer Clock and Prescaler */
	if ( basefreq )
		*basefreq = priv->base_freq;
	if ( tickrate )
		*tickrate = timer->tregs->reload + 1;
}

static int gptimer_tlib_set_freq(struct tlib_dev *hand, unsigned int tickrate)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;

	timer->tregs->reload = tickrate - 1;

	/*Check that value was allowed (Timer may not be as wide as expected)*/
	if ( timer->tregs->reload != (tickrate - 1) )
		return -1;
	else
		return 0;
}

static void gptimer_tlib_irq_reg(struct tlib_dev *hand, tlib_isr_t func, void *data)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);

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

	timer->tregs->ctrl |= GPTIMER_CTRL_IE;
}

static void gptimer_tlib_irq_unreg(struct tlib_dev *hand, tlib_isr_t func, void *data)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;
	struct gptimer_priv *priv = priv_from_timer(timer);

	/* Turn off IRQ at source, unregister IRQ handler */
	timer->tregs->ctrl &= ~GPTIMER_CTRL_IE;

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
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;
	unsigned int ctrl;

	/* Load the selected frequency before starting Frequency */
	ctrl = GPTIMER_CTRL_LD | GPTIMER_CTRL_EN;
	if ( once == 0 )
		ctrl |= GPTIMER_CTRL_RS; /* Restart Timer */
	timer->tregs->ctrl = ctrl | (timer->tregs->ctrl & timer->irq_ack_mask &
			     ~GPTIMER_CTRL_RS);
}

static void gptimer_tlib_stop(struct tlib_dev *hand)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;

	/* Load the selected Frequency */
	timer->tregs->ctrl &= ~(GPTIMER_CTRL_EN|GPTIMER_CTRL_IP);
}

static void gptimer_tlib_restart(struct tlib_dev *hand)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;

	timer->tregs->ctrl |= GPTIMER_CTRL_LD | GPTIMER_CTRL_EN;
}

static void gptimer_tlib_get_counter(
	struct tlib_dev *hand,
	unsigned int *counter)
{
	struct gptimer_timer *timer = (struct gptimer_timer *)hand;

	*counter = timer->tregs->value;
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
};
