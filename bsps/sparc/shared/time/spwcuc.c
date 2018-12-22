/*  SPWCUC - SpaceWire - CCSDS unsegmented Code Transfer Protocol GRLIB core
 *  register driver interface.
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

#include <grlib/spwcuc.h>

#include <grlib/grlib_impl.h>

/* Private structure of SPWCUC driver. */
struct spwcuc_priv {
	struct drvmgr_dev *dev;
	struct spwcuc_regs *regs;
	int open;
	
	spwcuc_isr_t user_isr;
	void *user_isr_arg;

	struct spwcuc_stats stats;
};

void spwcuc_isr(void *data);

struct amba_drv_info spwcuc_drv_info;

/* Hardware Reset of SPWCUC */
static int spwcuc_hw_reset(struct spwcuc_priv *priv)
{
	struct spwcuc_regs *r = priv->regs;
	int i = 1000;

	r->control = 1;

	while ((r->control & 1) && i > 0) {
		i--;
	}

	spwcuc_clear_irqs(priv, -1);

	return i ? 0 : -1;
}

int spwcuc_reset(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return spwcuc_hw_reset(priv);
}

void *spwcuc_open(int minor)
{
	struct spwcuc_priv *priv;
	struct drvmgr_dev *dev;

	/* Get Device from Minor */
	if ( drvmgr_get_dev(&spwcuc_drv_info.general, minor, &dev) ) {
		return NULL;
	}

	priv = dev->priv;
	if ( (priv == NULL) || priv->open )
		return NULL;

	/* Set initial state of software */
	priv->open = 1;

	/* Clear Statistics */
	spwcuc_clr_stats(priv);
	priv->user_isr = NULL;
	priv->user_isr_arg = NULL;

	return priv;
}

void spwcuc_close(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	if ( priv->open == 0 )
		return;

	/* Reset Hardware */
	spwcuc_hw_reset(priv);

	priv->open = 0;	
}

void spwcuc_int_enable(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	/* Register and Enable Interrupt at Interrupt controller */
	drvmgr_interrupt_register(priv->dev, 0, "spwcuc", spwcuc_isr, priv);
}

void spwcuc_int_disable(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	/* Enable Interrupt at Interrupt controller */
	drvmgr_interrupt_unregister(priv->dev, 0, spwcuc_isr, priv);
}

void spwcuc_clr_stats(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	memset(&priv->stats, 0, sizeof(priv->stats));
}

void spwcuc_get_stats(void *spwcuc, struct spwcuc_stats *stats)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	memcpy(stats, &priv->stats, sizeof(priv->stats));
}

/* Configure the spwcuc core */
void spwcuc_config(void *spwcuc, struct spwcuc_cfg *cfg)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;
	struct spwcuc_regs *r = priv->regs;

	r->config = (cfg->sel_out & 0x1f)   << 28 |
         	    (cfg->sel_in & 0x1f)    << 24 |
		    (cfg->mapping & 0x1f)   << 16 |
		    (cfg->tolerance & 0x1f) << 8  |
		    (cfg->tid & 0x7)        << 4  |
  		    (cfg->ctf & 1)          << 1  |
		    (cfg->cp & 1);

	r->control = (cfg->txen & 1)      << 1 |
		     (cfg->rxen & 1)      << 2 |
		     (cfg->pktsyncen & 1) << 3 |
		     (cfg->pktiniten & 1) << 4 |
		     (cfg->pktrxen & 1)   << 5;

	r->dla = (cfg->dla_mask & 0xff)<<8 | (cfg->dla & 0xff);

	r->pid = cfg->pid;

	r->offset = cfg->offset;
}

/* Return elapsed coarse time */
unsigned int spwcuc_get_et_coarse(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return priv->regs->etct;
}

/* Return elapsed fine time */
unsigned int spwcuc_get_et_fine(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return (priv->regs->etft & 0xffffff) >> 8;
}

/* Return elapsed time (coarse and fine) */
unsigned long long spwcuc_get_et(void *spwcuc)
{
	return (((unsigned long long)spwcuc_get_et_coarse(spwcuc)) << 24) | spwcuc_get_et_fine(spwcuc);
}

/* Return next elapsed coarse time (for use when sending SpW time packet) */
unsigned int spwcuc_get_next_et_coarse(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return priv->regs->etct_next;
}

/* Return next elapsed fine time (for use when sending SpW time packet) */
unsigned int spwcuc_get_next_et_fine(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return (priv->regs->etft_next & 0xffffff) >> 8;
}

/* Return next elapsed time (for use when sending SpW time packet) */
unsigned long long spwcuc_get_next_et(void *spwcuc)
{
	return (((unsigned long long)spwcuc_get_next_et_coarse(spwcuc)) << 24) | spwcuc_get_next_et_fine(spwcuc);
}

/* Force/Set the elapsed time (coarse 32-bit and fine 24-bit) by writing the
 * T-Field Time Packet Registers then the FORCE, NEW and INIT bits. 
 * The latter three are needed for the ET to be set with the new value.
 */
void spwcuc_force_et(void *spwcuc, unsigned long long time)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;
	struct spwcuc_regs *regs = priv->regs;

	regs->etft_next = (time & 0xffffff) << 8;
	regs->etct_next = (time >> 24) & 0xffffffff;
	regs->pkt_pf_crc = (1 << 29) | (1 << 30) | (1 << 31);
}

/* Return received (from time packet) elapsed coarse time */
unsigned int spwcuc_get_tp_et_coarse(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return priv->regs->pkt_ct;
}

/* Return received (from time packet) elapsed fine time */
unsigned int spwcuc_get_tp_et_fine(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return (priv->regs->pkt_ft & 0xffffff) >> 8;
}

/* Return received (from time packet) elapsed time (coarse and fine) */
unsigned long long spwcuc_get_tp_et(void *spwcuc)
{
	return (((unsigned long long)spwcuc_get_tp_et_coarse(spwcuc)) << 24) | spwcuc_get_tp_et_fine(spwcuc);
}

/* Clear interrupts */
void spwcuc_clear_irqs(void *spwcuc, int irqs)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	priv->regs->picr = irqs;
}

/* Enable interrupts */
void spwcuc_enable_irqs(void *spwcuc, int irqs)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	priv->regs->imr  = irqs;
}

struct spwcuc_regs *spwcuc_get_regs(void *spwcuc)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	return priv->regs;
}

void spwcuc_int_register(void *spwcuc, spwcuc_isr_t func, void *data)
{
	struct spwcuc_priv *priv = (struct spwcuc_priv *)spwcuc;

	priv->user_isr = func;
	priv->user_isr_arg = data;
}

void spwcuc_isr(void *data)
{
	struct spwcuc_priv *priv = data;
	struct spwcuc_stats *stats = &priv->stats;
	unsigned int pimr = priv->regs->pimr;

	stats->nirqs++;

	if (pimr & PKT_INIT_IRQ)
		stats->pkt_init++;
	if (pimr & PKT_ERR_IRQ)
		stats->pkt_err++;
	if (pimr & PKT_RX_IRQ)
		stats->pkt_rx++;
	if (pimr & WRAP_ERR_IRQ)
		stats->wraperr++;
	if (pimr & WRAP_IRQ)
		stats->wrap++;
	if (pimr & SYNC_ERR_IRQ)
		stats->syncerr++;
	if (pimr & SYNC_IRQ)
		stats->sync++;
	if (pimr & TOL_ERR_IRQ)
		stats->tolerr++;
	if (pimr & TICK_RX_ERR_IRQ)
		stats->tick_rx_error++;
	if (pimr & TICK_RX_WRAP_IRQ)
		stats->tick_rx_wrap++;
	if (pimr & TICK_RX_IRQ)
		stats->tick_rx++;
	if (pimr & TICK_TX_WRAP_IRQ)
		stats->tick_tx_wrap++;
	if (pimr & TICK_TX_IRQ)
		stats->tick_tx++;

	/* Let user Handle Interrupt */
	if ( priv->user_isr )
		priv->user_isr(pimr, priv->user_isr_arg);
}

/*** INTERFACE TO DRIVER MANAGER ***/

static int spwcuc_init2(struct drvmgr_dev *dev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	struct spwcuc_priv *priv;
	struct spwcuc_regs *regs;

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
	regs = (struct spwcuc_regs *)pnpinfo->apb_slv->start;

	priv->regs = regs;

	spwcuc_hw_reset(priv);

	return 0;
}

struct drvmgr_drv_ops spwcuc_ops =
{
	{NULL, spwcuc_init2, NULL, NULL},
	NULL,
	NULL
};

struct amba_dev_id spwcuc_ids[] =
{
	{VENDOR_GAISLER, GAISLER_SPWCUC},
	{0, 0}	/* Mark end of table */
};

struct amba_drv_info spwcuc_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_SPWCUC_ID,/* Driver ID */
		"SPWCUC_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&spwcuc_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&spwcuc_ids[0]
};

/* Register the SPWCUC Driver */
void spwcuc_register(void)
{
	drvmgr_drv_register(&spwcuc_drv_info.general);
}
