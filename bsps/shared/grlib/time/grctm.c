/*  GRCTM - CCSDS Time Manager - register driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <stdlib.h>
#include <string.h>

#include <grlib/grctm.h>

#include <grlib/grlib_impl.h>

/* Private structure of GRCTM driver */
struct grctm_priv {
	struct drvmgr_dev *dev;
	struct grctm_regs *regs;
	int open;

	grctm_isr_t user_isr;
	void *user_isr_arg;

	struct grctm_stats stats;
};

void grctm_isr(void *data);

struct amba_drv_info grctm_drv_info;

void *grctm_open(int minor)
{
	struct grctm_priv *priv;
	struct drvmgr_dev *dev;

	/* Get Device from Minor */
	if ( drvmgr_get_dev(&grctm_drv_info.general, minor, &dev) ) {
		return NULL;
	}

	priv = dev->priv;
	if ( (priv == NULL) || priv->open )
		return NULL;

	/* Set initial state of software */
	priv->open = 1;

	/* Clear Statistics */
	grctm_clr_stats(priv);
	priv->user_isr = NULL;
	priv->user_isr_arg = NULL;

	return priv;
}

void grctm_close(void *grctm)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	if ( priv->open == 0 )
		return;

	/* Reset Hardware */
	grctm_reset(priv);

	priv->open = 0;
}

/* Hardware Reset of GRCTM */
int grctm_reset(void *grctm)
{
	struct grctm_priv *priv = grctm;
	struct grctm_regs *r = priv->regs;

	r->grr = 0x55000001;

	int i = 1000;
	while ((r->grr & 1) && i > 0) {
		i--;
	}

	return i ? 0 : -1;
}

void grctm_int_enable(void *grctm)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	/* Register and Enable Interrupt at Interrupt controller */
	drvmgr_interrupt_register(priv->dev, 0, "grctm", grctm_isr, priv);
}

void grctm_int_disable(void *grctm)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	/* Enable Interrupt at Interrupt controller */
	drvmgr_interrupt_unregister(priv->dev, 0, grctm_isr, priv);
}

void grctm_clr_stats(void *grctm)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	memset(&priv->stats, 0, sizeof(priv->stats));
}

void grctm_get_stats(void *grctm, struct grctm_stats *stats)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	memcpy(stats, &priv->stats, sizeof(priv->stats));
}

/* Enable external synchronisation (from grctm) */
void grctm_enable_ext_sync(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr |= 0x55<<24 | 1<<9;
}

/* Disable external synchronisation (from grctm) */
void grctm_disable_ext_sync(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr &= ~((0xAA<<24) | 1<<9);
}

/* Enable TimeWire synchronisation */
void grctm_enable_tw_sync(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr |= 0x55<<24 | 1<<8;
}

/* Disable TimeWire synchronisation */
void grctm_disable_tw_sync(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr &= ~((0xAA<<24) | 1<<8);
}

/* Disable frequency synthesizer from driving ET */
void grctm_disable_fs(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr |= 0x55<<24 | 1<<7;
}

/* Enable frequency synthesizer to drive ET */
void grctm_enable_fs(void *grctm)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr &= ~((0xAA<<24) | 1<<7);
}

/* Return elapsed coarse time */
unsigned int grctm_get_et_coarse(void *grctm)
{
	struct grctm_priv *priv = grctm;

	return priv->regs->etcr;
}

/* Return elapsed fine time */
unsigned int grctm_get_et_fine(void *grctm)
{
	struct grctm_priv *priv = grctm;

	return (priv->regs->etfr & 0xffffff00) >> 8;
}

/* Return elapsed time (coarse and fine) */
unsigned long long grctm_get_et(void *grctm)
{
	return (((unsigned long)grctm_get_et_coarse(grctm)) << 24) | grctm_get_et_fine(grctm);
}


/* Return 1 if specified datation has been latched */
int grctm_is_dat_latched(void *grctm, int dat)
{
	struct grctm_priv *priv = grctm;

	return (priv->regs->gsr >> dat) & 1;
}

/* Set triggering edge of datation input */
void grctm_set_dat_edge(void *grctm, int dat, int edge)
{
	struct grctm_priv *priv = grctm;

	priv->regs->gcr &= ~((0xAA<<24) | 1 << (10+dat));
	priv->regs->gcr |= 0x55<<24 | (edge&1) << (10+dat);
}

/* Return latched datation coarse time */
unsigned int grctm_get_dat_coarse(void *grctm, int dat)
{
	struct grctm_priv *priv = grctm;

	switch (dat) {
	case 0 : return priv->regs->dcr0; 
	case 1 : return priv->regs->dcr1;
	case 2 : return priv->regs->dcr2;
	default: return -1;
	}
}

/* Return latched datation fine time */
unsigned int grctm_get_dat_fine(void *grctm, int dat)
{
	struct grctm_priv *priv = grctm;

	switch (dat) {
	case 0 : return (priv->regs->dfr0 & 0xffffff00) >> 8; 
	case 1 : return (priv->regs->dfr1 & 0xffffff00) >> 8;
	case 2 : return (priv->regs->dfr2 & 0xffffff00) >> 8;
	default: return -1;
	}
}


/* Return latched datation ET */
unsigned long long grctm_get_dat_et(void *grctm, int dat)
{
	return (((unsigned long)grctm_get_dat_coarse(grctm, dat)) << 24) | 
		grctm_get_dat_fine(grctm, dat);
}


/* Return current pulse configuration */
unsigned int grctm_get_pulse_reg(void *grctm, int pulse)
{
	struct grctm_priv *priv = grctm;

	return priv->regs->pdr[pulse];
}

/* Set pulse register */
void grctm_set_pulse_reg(void *grctm, int pulse, unsigned int val)
{
	struct grctm_priv *priv = grctm;

	priv->regs->pdr[pulse] = val;
}

/* Configure pulse: pp = period, pw = width, pl = level, en = enable */
void grctm_cfg_pulse(void *grctm, int pulse, int pp, int pw, int pl, int en)
{
	grctm_set_pulse_reg(grctm, pulse, (pp&0xf)<<20 | (pw&0xf)<<16 | (pl&1)<<10 | (en&1)<<1);
}

/* Enable pulse output */
void grctm_enable_pulse(void *grctm, int pulse)
{
	struct grctm_priv *priv = grctm;

	priv->regs->pdr[pulse] |= 0x2;
}

/* Disable pulse output */
void grctm_disable_pulse(void *grctm, int pulse)
{
	struct grctm_priv *priv = grctm;

	priv->regs->pdr[pulse] &= ~0x2;
}

/* Clear interrupts */
void grctm_clear_irqs(void *grctm, int irqs)
{
	struct grctm_priv *priv = grctm;

	priv->regs->picr = irqs;
}

/* Enable interrupts */
void grctm_enable_irqs(void *grctm, int irqs)
{
	struct grctm_priv *priv = grctm;

	priv->regs->imr  = irqs;
}

/* Set Frequency synthesizer increment */
void grctm_set_fs_incr(void *grctm, int incr)
{
	struct grctm_priv *priv = grctm;

	priv->regs->fsir  = incr;
}

/* Set ET increment */
void grctm_set_et_incr(void *grctm, int incr)
{
	struct grctm_priv *priv = grctm;

	priv->regs->etir  = incr;
}


void grctm_isr(void *data)
{
	struct grctm_priv *priv = data;
	struct grctm_stats *stats = &priv->stats;
	unsigned int pimr = priv->regs->pimr;

	if ( pimr == 0 )
		return;

	stats->nirqs++;
	if (pimr & PULSE0_IRQ )
		stats->pulse++;

	/* Let user Handle Interrupt */
	if ( priv->user_isr )
		priv->user_isr(pimr, priv->user_isr_arg);
}

struct grctm_regs *grctm_get_regs(void *grctm)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	return priv->regs;
}

void grctm_int_register(void *grctm, grctm_isr_t func, void *data)
{
	struct grctm_priv *priv = (struct grctm_priv *)grctm;

	priv->user_isr = func;
	priv->user_isr_arg = data;
}

/*** INTERFACE TO DRIVER MANAGER ***/

static int grctm_init2(struct drvmgr_dev *dev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	struct grctm_priv *priv;
	struct grctm_regs *regs;

	priv = grlib_calloc(1, sizeof(*priv));
	if ( priv == NULL )
		return -1;
	priv->dev = dev;
	dev->priv = priv;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	regs = (struct grctm_regs *)pnpinfo->ahb_slv->start[0];

	priv->regs = regs;

	grctm_reset(priv);

	return 0;
}

struct drvmgr_drv_ops grctm_ops =
{
	{NULL, grctm_init2, NULL, NULL},
	NULL,
	NULL
};

struct amba_dev_id grctm_ids[] =
{
	{VENDOR_GAISLER, GAISLER_GRCTM},
	{0, 0}	/* Mark end of table */
};

struct amba_drv_info grctm_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRCTM_ID,	/* Driver ID */
		"GRCTM_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grctm_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&grctm_ids[0]
};

/* Register the grctm Driver */
void grctm_register(void)
{
	drvmgr_drv_register(&grctm_drv_info.general);
}
