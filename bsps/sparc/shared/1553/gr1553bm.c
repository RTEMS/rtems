/*  GR1553B BM driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <string.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/gr1553b.h>
#include <grlib/gr1553bm.h>

#include <grlib/grlib_impl.h>

#define GR1553BM_WRITE_MEM(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BM_READ_MEM(adr) (*(volatile uint32_t *)(adr))

#define GR1553BM_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BM_READ_REG(adr) (*(volatile uint32_t *)(adr))

struct gr1553bm_priv {
	struct drvmgr_dev **pdev;
	struct gr1553b_regs *regs;
	SPIN_DECLARE(devlock);

	void *buffer;
	unsigned int buffer_base_hw;
	unsigned int buffer_base;
	unsigned int buffer_end;
	unsigned int buffer_size;
	unsigned int read_pos;
	int started;
	struct gr1553bm_config cfg;

	/* Time updated by IRQ when 24-bit Time counter overflows */
	volatile uint64_t time;
};

void gr1553bm_isr(void *data);

/* Default Driver configuration */
struct gr1553bm_config gr1553bm_default_config =
{
	/* Highest resolution, use Time overflow IRQ to track */
	.time_resolution = 0,
	.time_ovf_irq = 1,

	/* No filtering, log all */
	.filt_error_options = GR1553BM_ERROPTS_ALL,
	.filt_rtadr = 0xffffffff,
	.filt_subadr = 0xffffffff,
	.filt_mc = 0x0007ffff,

	/* 128Kbyte dynamically allocated buffer. */
	.buffer_size = 128*1024,
	.buffer_custom = NULL,
};

void gr1553bm_register(void)
{
	/* The BM driver rely on the GR1553B Driver */
	gr1553_register();
}

static void gr1553bm_hw_start(struct gr1553bm_priv *priv)
{
	SPIN_IRQFLAGS(irqflags);

	/* Enable IRQ source and mark running state */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	priv->started = 1;

	/* Clear old IRQ flags */
	priv->regs->irq = GR1553B_IRQ_BMD | GR1553B_IRQ_BMTOF;

	/* Unmask IRQ sources */
	if ( priv->cfg.time_ovf_irq ) {
		priv->regs->imask |= GR1553B_IRQEN_BMDE | GR1553B_IRQEN_BMTOE;
	} else {
		priv->regs->imask |= GR1553B_IRQEN_BMDE;
	}

	/* Start logging */
	priv->regs->bm_ctrl =
		(priv->cfg.filt_error_options & 
		(GR1553B_BM_CTRL_MANL|GR1553B_BM_CTRL_UDWL|GR1553B_BM_CTRL_IMCL))
		| GR1553B_BM_CTRL_BMEN;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

static void gr1553bm_hw_stop(struct gr1553bm_priv *priv)
{
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Stop Logging */
	priv->regs->bm_ctrl = 0;

	/* Stop IRQ source */
	priv->regs->imask &= ~(GR1553B_IRQEN_BMDE|GR1553B_IRQEN_BMTOE);

	/* Clear IRQ flags */
	priv->regs->irq = GR1553B_IRQ_BMD | GR1553B_IRQ_BMTOF;

	priv->started = 0;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

/* Open device by number */
void *gr1553bm_open(int minor)
{
	struct drvmgr_dev **pdev = NULL;
	struct gr1553bm_priv *priv = NULL;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;

	/* Allocate requested device */
	pdev = gr1553_bm_open(minor);
	if ( pdev == NULL )
		goto fail;

	priv = grlib_calloc(1, sizeof(*priv));
	if ( priv == NULL )
		goto fail;

	/* Init BC device */
	priv->pdev = pdev;
	(*pdev)->priv = priv;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)(*pdev)->businfo;
	pnpinfo = &ambadev->info;
	priv->regs = (struct gr1553b_regs *)pnpinfo->apb_slv->start;
	SPIN_INIT(&priv->devlock, "gr1553bm");

	/* Start with default configuration */
	priv->cfg = gr1553bm_default_config;

	/* Unmask IRQs */
	gr1553bm_hw_stop(priv);

	return priv;

fail:
	if ( pdev )
		gr1553_bm_close(pdev);
	if ( priv )
		free(priv);
	return NULL;
}

/* Close previously */
void gr1553bm_close(void *bm)
{
	struct gr1553bm_priv *priv = bm;

	if ( priv->started ) {
		gr1553bm_stop(bm);
	}

	if ( (priv->cfg.buffer_custom == NULL) && priv->buffer )
		free(priv->buffer);

	gr1553_bm_close(priv->pdev);
	free(priv);
}

/* Configure the BM driver */
int gr1553bm_config(void *bm, struct gr1553bm_config *cfg)
{
	struct gr1553bm_priv *priv = bm;

	if ( priv->started )
		return -1;

	/* Check Config validity? */
/*#warning IMPLEMENT.*/

	/* Free old buffer if dynamically allocated */
	if ( (priv->cfg.buffer_custom == NULL) && priv->buffer ) {
		free(priv->buffer);
		priv->buffer = NULL;
	}
	priv->buffer_size = cfg->buffer_size & ~0x7; /* on 8 byte bounadry */
	if ((unsigned int)cfg->buffer_custom & 1) {
		/* Custom Address Given in Remote address. We need
		 * to convert it intoTranslate into Hardware a
		 * hardware accessible address
		 */
		priv->buffer_base_hw = (unsigned int)cfg->buffer_custom & ~1;
		priv->buffer = cfg->buffer_custom;
		drvmgr_translate_check(
			*priv->pdev,
			DMAMEM_TO_CPU,
			(void *)priv->buffer_base_hw,
			(void **)&priv->buffer_base,
			priv->buffer_size);
	} else {
		if (cfg->buffer_custom == NULL) {
			/* Allocate new buffer dynamically */
			priv->buffer = grlib_malloc(priv->buffer_size + 8);
			if (priv->buffer == NULL)
				return -1;
		} else {
			/* Address given in CPU accessible address, no
			 * translation required.
			 */
			priv->buffer = cfg->buffer_custom;
		}
		/* Align to 16 bytes */
		priv->buffer_base = ((unsigned int)priv->buffer + (8-1)) &
					~(8-1);
		/* Translate address of buffer base into address that Hardware must
		 * use to access the buffer.
		 */
		drvmgr_translate_check(
			*priv->pdev,
			CPUMEM_TO_DMA,
			(void *)priv->buffer_base,
			(void **)&priv->buffer_base_hw,
			priv->buffer_size);
		
	}

	/* Copy valid config */
	priv->cfg = *cfg;

	return 0;
}

/* Start logging */
int gr1553bm_start(void *bm)
{
	struct gr1553bm_priv *priv = bm;

	if ( priv->started )
		return -1;
	if ( priv->buffer == NULL )
		return -2;

	/* Start at Time = 0 */
	priv->regs->bm_ttag = 
		priv->cfg.time_resolution << GR1553B_BM_TTAG_RES_BIT;

	/* Configure Filters */
	priv->regs->bm_adr = priv->cfg.filt_rtadr;
	priv->regs->bm_subadr = priv->cfg.filt_subadr;
	priv->regs->bm_mc = priv->cfg.filt_mc;

	/* Set up buffer */
	priv->regs->bm_start = priv->buffer_base_hw;
	priv->regs->bm_end = priv->buffer_base_hw + priv->cfg.buffer_size - 4;
	priv->regs->bm_pos = priv->buffer_base_hw;
	priv->read_pos = priv->buffer_base;
	priv->buffer_end = priv->buffer_base + priv->cfg.buffer_size;

	/* Register ISR handler and unmask IRQ source at IRQ controller */
	if (drvmgr_interrupt_register(*priv->pdev, 0, "gr1553bm", gr1553bm_isr, priv))
		return -3;

	/* Start hardware and set priv->started */
	gr1553bm_hw_start(priv);

	return 0;
}

/* Stop logging */
void gr1553bm_stop(void *bm)
{
	struct gr1553bm_priv *priv = bm;

	/* Stop Hardware */
	gr1553bm_hw_stop(priv);

	/* At this point the hardware must be stopped and IRQ 
	 * sources unmasked.
	 */

	/* Unregister ISR handler and unmask 1553 IRQ source at IRQ ctrl */
	drvmgr_interrupt_unregister(*priv->pdev, 0, gr1553bm_isr, priv);
}

int gr1553bm_started(void *bm)
{
	return ((struct gr1553bm_priv *)bm)->started;
}

/* Get 64-bit 1553 Time.
 *
 * Update software time counters and return the current time.
 */
void gr1553bm_time(void *bm, uint64_t *time)
{
	struct gr1553bm_priv *priv = bm;
	unsigned int hwtime, hwtime2;

resample:
	if ( priv->started && (priv->cfg.time_ovf_irq == 0) ) {
		/* Update Time overflow counter. The carry bit from Time counter
		 * is located in IRQ Flag.
		 *
		 * When IRQ is not used this function must be called often
		 * enough to avoid that the Time overflows and the carry
		 * bit is already set. The frequency depends on the Time
		 * resolution.
		 */
		if ( priv->regs->irq & GR1553B_IRQ_BMTOF ) {
			/* Clear carry bit */
			priv->regs->irq = GR1553B_IRQ_BMTOF;
			priv->time += (GR1553B_BM_TTAG_VAL + 1);
		}
	}

	/* Report current Time, even if stopped */
	hwtime = priv->regs->bm_ttag & GR1553B_BM_TTAG_VAL;
	if ( time )
		*time = priv->time | hwtime;

	if ( priv->cfg.time_ovf_irq ) {
		/* Detect wrap around */
		hwtime2 = priv->regs->bm_ttag & GR1553B_BM_TTAG_VAL;
		if ( hwtime > hwtime2 ) {
			/* priv->time and hwtime may be out of sync if
			 * IRQ updated priv->time just after bm_ttag was read 
			 * here, we resample if we detect inconsistancy.
			 */
			goto resample;
		}
	}
}

/* Number of entries available in DMA buffer */
int gr1553bm_available(void *bm, int *nentries)
{
	struct gr1553bm_priv *priv = bm;
	unsigned int top, bot, pos;

	if ( !priv->started )
		return -1;

	/* Get BM posistion in log */
	pos = priv->regs->bm_pos;

	/* Convert into CPU accessible address */
	pos = priv->buffer_base + (pos - priv->buffer_base_hw);

	if ( pos >= priv->read_pos ) {
		top = (pos - priv->read_pos)/sizeof(struct gr1553bm_entry);
		bot = 0;
	} else {
		top = (priv->buffer_end - priv->read_pos)/sizeof(struct gr1553bm_entry);
		bot = (pos - priv->buffer_base)/sizeof(struct gr1553bm_entry);
	}

	if ( nentries )
		*nentries = top+bot;

	return 0;
}

/* Read a maximum number of entries from LOG buffer. */
int gr1553bm_read(void *bm, struct gr1553bm_entry *dst, int *max)
{
	struct gr1553bm_priv *priv = bm;
	unsigned int dest, pos, left, newPos, len;
	unsigned int topAdr, botAdr, topLen, botLen;

	if ( !priv || !priv->started )
		return -1;

	left = *max;
	pos = priv->regs->bm_pos & ~0x7;

	/* Convert into CPU accessible address */
	pos = priv->buffer_base + (pos - priv->buffer_base_hw);

	if ( (pos == priv->read_pos) || (left < 1) ) {
		/* No data available */
		*max = 0;
		return 0;
	}
	newPos = 0;

	/* Addresses and lengths of BM log buffer */
	if ( pos >= priv->read_pos ) {
		/* Read Top only */
		topAdr = priv->read_pos;
		botAdr = 0;
		topLen = (pos - priv->read_pos)/sizeof(struct gr1553bm_entry);
		botLen = 0;
	} else {
		/* Read Top and Bottom */
		topAdr = priv->read_pos;
		botAdr = priv->buffer_base;
		topLen = (priv->buffer_end - priv->read_pos)/sizeof(struct gr1553bm_entry);
		botLen = (pos - priv->buffer_base)/sizeof(struct gr1553bm_entry);
	}

	dest = (unsigned int)dst;
	if ( topLen > 0 ) {
		/* Copy from top area first */
		if ( topLen > left ) {
			len = left;
			left = 0;
		} else {
			len = topLen;
			left -= topLen;
		}
		newPos = topAdr + (len * sizeof(struct gr1553bm_entry));
		if ( newPos >= priv->buffer_end )
			newPos -= priv->buffer_size;
		if ( priv->cfg.copy_func ) {
			dest += priv->cfg.copy_func(
				dest,			/*Optional Destination*/
				(void *)topAdr, 	/* DMA start address */
				len,			/* Number of entries */
				priv->cfg.copy_func_arg /* Custom ARG */
				);
		} else {
			memcpy(	(void *)dest,
				(void *)topAdr,
				len * sizeof(struct gr1553bm_entry));
			dest += len * sizeof(struct gr1553bm_entry);
		}
	}

	if ( (botLen > 0) && (left > 0) ) {
		/* Copy bottom area last */
		if ( botLen > left ) {
			len = left;
			left = 0;
		} else {
			len = botLen;
			left -= botLen;
		}
		newPos = botAdr + (len * sizeof(struct gr1553bm_entry));

		if ( priv->cfg.copy_func ) {
			priv->cfg.copy_func(
				dest,			/*Optional Destination*/
				(void *)botAdr,		/* DMA start address */
				len,			/* Number of entries */
				priv->cfg.copy_func_arg /* Custom ARG */
				);
		} else {
			memcpy(	(void *)dest,
				(void *)botAdr,
				len * sizeof(struct gr1553bm_entry));
		}
	}

	/* Remember last read posistion in buffer */
	/*printf("New pos: 0x%08x (0x%08x), %d\n", newPos, priv->read_pos, *max - left);*/
	priv->read_pos = newPos;

	/* Return number of entries read */
	*max = *max - left;

	return 0;
}

/* Note: This is a shared interrupt handler, with BC/RT driver
 *       we must determine the cause of IRQ before handling it.
 */
void gr1553bm_isr(void *data)
{
	struct gr1553bm_priv *priv = data;
	uint32_t irqflag;

	/* Get Causes */
	irqflag = priv->regs->irq & (GR1553B_IRQ_BMD | GR1553B_IRQ_BMTOF);

	/* Check spurious IRQs */
	if ( (irqflag == 0) || (priv->started == 0) )
		return;

	if ( (irqflag & GR1553B_IRQ_BMTOF) && priv->cfg.time_ovf_irq ) {
		/* 1553 Time Over flow. Time is 24-bits */
		priv->time += (GR1553B_BM_TTAG_VAL + 1);

		/* Clear cause handled */
		priv->regs->irq = GR1553B_IRQ_BMTOF;
	}

	if ( irqflag & GR1553B_IRQ_BMD ) {
		/* BM DMA ERROR. Fatal error, we stop BM hardware and let
		 * user take care of it. From now on all calls will result
		 * in an error because the BM is stopped (priv->started=0).
		 */

		/* Clear cause handled */
		priv->regs->irq = GR1553B_IRQ_BMD;

		if ( priv->cfg.dma_error_isr )
			priv->cfg.dma_error_isr(data, priv->cfg.dma_error_arg);

		gr1553bm_hw_stop(priv);
	}
}
