/*
 *  SPICTRL SPI driver implmenetation
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <string.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/spictrl.h>
#include <grlib/ambapp.h>

#include <rtems/libi2c.h>

#include <grlib/grlib_impl.h>

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printk(x)
#define STATIC
#else
#define DBG(x...) 
#define STATIC static
#endif

/*** CAPABILITY REGISTER 0x00 ***/
#define SPICTRL_CAP_SSSZ_BIT	24
#define SPICTRL_CAP_AMODE_BIT	18
#define SPICTRL_CAP_ASELA_BIT	17
#define SPICTRL_CAP_SSEN_BIT	16
#define SPICTRL_CAP_FDEPTH_BIT	8
#define SPICTRL_CAP_REV_BIT	0

#define SPICTRL_CAP_SSSZ	(0xff << SPICTRL_CAP_SSSZ_BIT)
#define SPICTRL_CAP_AMODE	(1<<SPICTRL_CAP_AMODE_BIT)
#define SPICTRL_CAP_ASELA	(1<<SPICTRL_CAP_ASELA_BIT)
#define SPICTRL_CAP_SSEN	(1 << SPICTRL_CAP_SSEN_BIT)
#define SPICTRL_CAP_FDEPTH	(0xff << SPICTRL_CAP_FDEPTH_BIT)
#define SPICTRL_CAP_REV		(0xff << SPICTRL_CAP_REV_BIT)

/*** MODE REGISTER 0x20 ***/
#define SPICTRL_MODE_AMEN_BIT	31
#define SPICTRL_MODE_LOOP_BIT	30
#define SPICTRL_MODE_CPOL_BIT	29
#define SPICTRL_MODE_CPHA_BIT	28
#define SPICTRL_MODE_DIV16_BIT	27
#define SPICTRL_MODE_REV_BIT	26
#define SPICTRL_MODE_MS_BIT	25
#define SPICTRL_MODE_EN_BIT	24
#define SPICTRL_MODE_LEN_BIT	20
#define SPICTRL_MODE_PM_BIT	16
#define SPICTRL_MODE_ASEL_BIT	14
#define SPICTRL_MODE_FACT_BIT	13
#define SPICTRL_MODE_CG_BIT	7
#define SPICTRL_MODE_TAC_BIT	4

#define SPICTRL_MODE_AMEN	(1 << SPICTRL_MODE_AMEN_BIT)
#define SPICTRL_MODE_LOOP	(1 << SPICTRL_MODE_LOOP_BIT)
#define SPICTRL_MODE_CPOL	(1 << SPICTRL_MODE_CPOL_BIT)
#define SPICTRL_MODE_CPHA	(1 << SPICTRL_MODE_CPHA_BIT)
#define SPICTRL_MODE_DIV16	(1 << SPICTRL_MODE_DIV16_BIT)
#define SPICTRL_MODE_REV	(1 << SPICTRL_MODE_REV_BIT)
#define SPICTRL_MODE_MS		(1 << SPICTRL_MODE_MS_BIT)
#define SPICTRL_MODE_EN		(1 << SPICTRL_MODE_EN_BIT)
#define SPICTRL_MODE_LEN	(0xf << SPICTRL_MODE_LEN_BIT)
#define SPICTRL_MODE_PM		(0xf << SPICTRL_MODE_PM_BIT)
#define SPICTRL_MODE_ASEL	(1 << SPICTRL_MODE_ASEL_BIT)
#define SPICTRL_MODE_FACT	(1 << SPICTRL_MODE_FACT_BIT)
#define SPICTRL_MODE_CG		(0x1f << SPICTRL_MODE_CG_BIT)
#define SPICTRL_MODE_TAC	(0x1 << SPICTRL_MODE_TAC_BIT)

/*** EVENT REGISTER 0x24 ***/
#define SPICTRL_EVENT_AT_BIT	15
#define SPICTRL_EVENT_LT_BIT	14
#define SPICTRL_EVENT_OV_BIT	12
#define SPICTRL_EVENT_UN_BIT	11
#define SPICTRL_EVENT_MME_BIT	10
#define SPICTRL_EVENT_NE_BIT	9
#define SPICTRL_EVENT_NF_BIT	8

#define SPICTRL_EVENT_AT	(1 << SPICTRL_EVENT_AT_BIT)
#define SPICTRL_EVENT_LT	(1 << SPICTRL_EVENT_LT_BIT)
#define SPICTRL_EVENT_OV	(1 << SPICTRL_EVENT_OV_BIT)
#define SPICTRL_EVENT_UN	(1 << SPICTRL_EVENT_UN_BIT)
#define SPICTRL_EVENT_MME	(1 << SPICTRL_EVENT_MME_BIT)
#define SPICTRL_EVENT_NE	(1 << SPICTRL_EVENT_NE_BIT)
#define SPICTRL_EVENT_NF	(1 << SPICTRL_EVENT_NF_BIT)

/*** MASK REGISTER 0x28 ***/
#define SPICTRL_MASK_ATE_BIT	15
#define SPICTRL_MASK_LTE_BIT	14
#define SPICTRL_MASK_OVE_BIT	12
#define SPICTRL_MASK_UNE_BIT	11
#define SPICTRL_MASK_MMEE_BIT	10
#define SPICTRL_MASK_NEE_BIT	9
#define SPICTRL_MASK_NFE_BIT	8

#define SPICTRL_MASK_ATE	(1 << SPICTRL_MASK_ATE_BIT)
#define SPICTRL_MASK_LTE	(1 << SPICTRL_MASK_LTE_BIT)
#define SPICTRL_MASK_OVE	(1 << SPICTRL_MASK_OVE_BIT)
#define SPICTRL_MASK_UNE	(1 << SPICTRL_MASK_UNE_BIT)
#define SPICTRL_MASK_MMEE	(1 << SPICTRL_MASK_MMEE_BIT)
#define SPICTRL_MASK_NEE	(1 << SPICTRL_MASK_NEE_BIT)
#define SPICTRL_MASK_NFE	(1 << SPICTRL_MASK_NFE_BIT)

/*** COMMAND REGISTER 0x2c ***/
#define SPICTRL_CMD_LST_BIT	22
#define SPICTRL_CMD_LST		(1 << SPICTRL_CMD_LST_BIT)

/*** TRANSMIT REGISTER 0x30 ***/
#define SPICTRL_TX_TDATA_BIT	0
#define SPICTRL_TX_TDATA	0xffffffff

/*** RECEIVE REGISTER 0x34 ***/
#define SPICTRL_RX_RDATA_BIT	0
#define SPICTRL_RX_RDATA	0xffffffff

/*** SLAVE SELECT REGISTER 0x38 - VARIABLE ***/

/*** AM CONFIGURATION REGISTER 0x40 ***/
#define SPICTRL_AMCFG_ERPT_BIT		6
#define SPICTRL_AMCFG_SEQ_BIT		5
#define SPICTRL_AMCFG_STRICT_BIT	4
#define SPICTRL_AMCFG_OVTB_BIT		3
#define SPICTRL_AMCFG_OVDB_BIT		2
#define SPICTRL_AMCFG_ACT_BIT		1
#define SPICTRL_AMCFG_EACT_BIT		0

#define SPICTRL_AMCFG_ERPT	(1<<SPICTRL_AMCFG_ERPT_BIT)
#define SPICTRL_AMCFG_SEQ	(1<<SPICTRL_AMCFG_SEQ_BIT)
#define SPICTRL_AMCFG_STRICT	(1<<SPICTRL_AMCFG_STRICT_BIT)
#define SPICTRL_AMCFG_OVTB	(1<<SPICTRL_AMCFG_OVTB_BIT)
#define SPICTRL_AMCFG_OVDB	(1<<SPICTRL_AMCFG_OVDB_BIT)
#define SPICTRL_AMCFG_ACT	(1<<SPICTRL_AMCFG_ACT_BIT)
#define SPICTRL_AMCFG_EACT	(1<<SPICTRL_AMCFG_EACT_BIT)

struct spictrl_priv {
	rtems_libi2c_bus_t		i2clib_desc;
	struct drvmgr_dev	*dev;
	struct spictrl_regs		*regs;
	int				irq;
	int				minor;
	unsigned int			core_freq_hz;

	/* Driver */
	int				fdepth;
	int				bits_per_char;
	int				lsb_first;
	int				txshift;
	int				rxshift;
	unsigned int			idle_char;
	int				(*slvSelFunc)(void *regs, uint32_t addr, int select);
	
	/* Automated Periodic transfers */
	int				periodic_started;
	struct spictrl_ioctl_config	periodic_cfg;
};

/******************* Driver Manager Part ***********************/

int spictrl_device_init(struct spictrl_priv *priv);

int spictrl_init2(struct drvmgr_dev *dev);
int spictrl_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops spictrl_ops = 
{
	.init = {NULL, spictrl_init2, spictrl_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id spictrl_ids[] =
{
	{VENDOR_GAISLER, GAISLER_SPICTRL},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info spictrl_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_SPICTRL_ID,	/* Driver ID */
		"SPICTRL_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&spictrl_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&spictrl_ids[0]
};

void spictrl_register_drv (void)
{
	DBG("Registering SPICTRL driver\n");
	drvmgr_drv_register(&spictrl_drv_info.general);
}

int spictrl_init2(struct drvmgr_dev *dev)
{
	struct spictrl_priv *priv;

	DBG("SPICTRL[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int spictrl_init3(struct drvmgr_dev *dev)
{
	struct spictrl_priv *priv;
	char prefix[32];
	char devName[32];
	int rc;

	priv = (struct spictrl_priv *)dev->priv;

	/* Do initialization */

	/* Initialize i2c library */
	rc = rtems_libi2c_initialize();
	if (rc != 0) {
		DBG("SPICTRL: rtems_libi2c_initialize failed, exiting...\n");
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	/* Get frequency */
	if ( drvmgr_freq_get(dev, DEV_APB_SLV, &priv->core_freq_hz) ) {
		return DRVMGR_FAIL;
	}

	if ( spictrl_device_init(priv) ) {
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(devName, "/dev/spi%d", dev->minor_drv+1);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(devName, "/dev/%sspi%d", prefix, dev->minor_bus+1);
	}

	/* Register Bus for this Device */
	rc = rtems_libi2c_register_bus(devName, &priv->i2clib_desc);
	if (rc < 0) {
		DBG("SPICTRL: rtems_libi2c_register_bus(%s) failed\n", devName);
		free(dev->priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}
	priv->minor = rc;

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

STATIC rtems_status_code spictrl_libi2c_send_addr(rtems_libi2c_bus_t *bushdl,
					     uint32_t addr, int rw);

/* Set as high frequency of SCK as possible but not higher than 
 * requested frequency (freq).
 */
static int spictrl_set_freq(struct spictrl_priv *priv, unsigned int freq)
{
	unsigned int core_freq_hz = priv->core_freq_hz;
	unsigned int lowest_freq_possible;
	unsigned int div, div16, pm, fact;

	/* Lowest possible when DIV16 is set and PM is 0xf */
	lowest_freq_possible = core_freq_hz / (16 * 4 * (0xf + 1));

	if ( freq < lowest_freq_possible ) {
		DBG("SPICTRL: TOO LOW FREQ %u, CORE FREQ %u, LOWEST FREQ %u\n", 
			freq, core_freq_hz, lowest_freq_possible);
		return -1;
	}

	div = ((core_freq_hz / 2) + (freq-1)) / freq;
	DBG("SPICTRL: DIV=%d, FREQ=%d\n", div, freq);

	/* Is DIV16 neccessary? */
	if ( div > 16 ) {
		div = (div + (16 - 1)) / 16;
		div16 = 1;
	} else {
		div16 = 0;
	}

	if ( div > 0xf ) {
		fact = 0; /* FACT adds an factor /2 */
		div = (div + (2 - 1)) / 2;
	} else {
		fact = 1;
	}

	pm = div-1;

	/* Update hardware */
	priv->regs->mode = 
		(priv->regs->mode & ~(SPICTRL_MODE_PM|SPICTRL_MODE_DIV16|SPICTRL_MODE_FACT)) | 
		(pm << SPICTRL_MODE_PM_BIT) | (div16 << SPICTRL_MODE_DIV16_BIT) |
		(fact << SPICTRL_MODE_FACT_BIT);

	DBG("SPICTRL: Effective bit rate %u (requested %u), PM: %x, FACT: %d, div16: %x, core_freq: %u\n",
		core_freq_hz / (2 * (fact ? 1 : 2) * (div) * (div16 ? 16 : 1)),
		freq, pm, fact, div16, core_freq_hz);

	return 0;
}

/* Start Automated Periodic transfers, after this call read can be done */
static int spictrl_start_periodic(struct spictrl_priv *priv)
{
	struct spictrl_ioctl_config *cfg = &priv->periodic_cfg;
	unsigned int am_cfg;

	/* Clear the events */
	priv->regs->event = 0xffffffff;

	/* Enable core */
	priv->regs->mode |= SPICTRL_MODE_EN | SPICTRL_MODE_MS;

	/* Update hardware config from flags and period */
	priv->regs->am_period = cfg->period;

	/* Remove SPICTRL_PERIOD_FLAGS_ASEL and ACT bit and shift into posistion */
	am_cfg = (cfg->period_flags & 0x1f8) >> 1;
	priv->regs->am_cfg = am_cfg;

	/* Start automated periodic transfers */
	if ( cfg->period_flags & SPICTRL_PERIOD_FLAGS_EACT ) {
		/* Enable external triggering */
		priv->regs->am_cfg = am_cfg | SPICTRL_AMCFG_EACT;
	} else {
		/* Activate periodic transfers */
		priv->regs->am_cfg = am_cfg | SPICTRL_AMCFG_ACT;
	}

	return 0;
}

/* Stop Automated Periodic transfers */
static void spictrl_stop_periodic(struct spictrl_priv *priv)
{
	priv->regs->am_cfg = 0;
}

/* Return the status of the SPI controller (the event register), 
 * it may be needed in periodic mode to look at the Not Full bit (NF)
 * in order not to hang in an infinte loop when read is called.
 */
static inline unsigned int spictrl_status(struct spictrl_priv *priv)
{
	return priv->regs->event;
}

static int spictrl_read_periodic(
	struct spictrl_priv *priv,
	struct spictrl_period_io *rarg)
{
	int i, rxi, rxshift, bits_per_char, reg;
	unsigned int rx_word, mask;
	void *rxbuf;

	if ( rarg->options & 0x1 ) {
		/* Read mask registers */
		for (i=0; i<4; i++) {
			rarg->masks[i] = priv->regs->am_mask[i];
		}
	}

	if ( rarg->options & 0x2 ) {
		/* Read receive registers (after updating masks so that the caller can
		 * read current buffer without knowning of actual register mask).
		 */

		/* If not started we could be hanging here forever. */
		if ( !priv->periodic_started )
			return -1;

		rxshift = priv->rxshift;
		bits_per_char = priv->bits_per_char;
		rx_word = 0;

		rxbuf = rarg->data;
		if ( !rxbuf ) {
			/* If no data pointer specified we cannot copy data... */
			return -1;
		}

		/* Wait until all data is available (if started) */
		while ( (priv->regs->event & SPICTRL_EVENT_NE) == 0 ) {
			;
		}

		rxi = 0;
		for (i=0; i<4; i++) {
			mask = rarg->masks[i];
			reg = 0;
			while ( mask ) {
				if ( mask & 1 ) {
					/* Update Register */
					rx_word = priv->regs->am_rx[i*32 + reg] >> rxshift;

					if ( bits_per_char <= 8 ) {
						*((unsigned char *)rxbuf + rxi) = rx_word;
					} else if ( bits_per_char <= 16 ) {
						*((unsigned short *)rxbuf + rxi) = rx_word;
					} else {
						*((unsigned int *)rxbuf + rxi) = rx_word;
					}
					rxi++;
				}

				mask = mask>>1;
				reg++;
			}
		}
	}

	return 0;
}

static int spictrl_write_periodic(
	struct spictrl_priv *priv,
	struct spictrl_period_io *warg)
{
	int i, txi, txshift, bits_per_char, reg;
	unsigned int tx_word, mask;
	void *txbuf;

	if ( warg->options & 0x2 ) {

		/* Make sure core is enabled, otherwise TX registers writes are lost */
		priv->regs->mode |= SPICTRL_MODE_EN;

		/* Update Transmit registers (before updating masks so that we do not
		 * transmit invalid data)
		 */

		txshift = priv->txshift;
		bits_per_char = priv->bits_per_char;
		tx_word = 0;

		txbuf = warg->data;
		if ( !txbuf ) {
			/* If no data pointer specified we fill up with
			 * idle chars.
			 */
			tx_word = priv->idle_char << txshift;
		}

		txi = 0;
		for (i=0; i<4; i++) {
			mask = warg->masks[i];
			reg = 0;
			while ( mask ) {
				if ( mask & 1 ) {
					if ( txbuf ) {
						if ( bits_per_char <= 8 ) {
							tx_word = *((unsigned char *)txbuf + txi);
						} else if ( bits_per_char <= 16 ) {
							tx_word = *((unsigned short *)txbuf + txi);
						} else {
							tx_word = *((unsigned int *)txbuf + txi);
						}
						tx_word = tx_word << txshift;
						txi++;
					}

					/* Update Register */
					DBG("WRITE 0x%08x to 0x%08x\n", tx_word, &priv->regs->am_tx[i*32 + reg]);
					priv->regs->am_tx[i*32 + reg] = tx_word;
				}

				mask = mask>>1;
				reg++;
			}
		}
	}

	if ( warg->options & 0x1 ) {
		/* Update mask registers */
		for (i=0; i<4; i++) {
			DBG("WRITE 0x%08x to 0x%08x (MSK%d)\n", warg->masks[i], &priv->regs->am_mask[i], i);
			priv->regs->am_mask[i] = warg->masks[i];
		}
	}

	return 0;
}

static int spictrl_read_write(
	struct spictrl_priv *priv,
	void *rxbuf, 
	void *txbuf,
	int len)
{
	unsigned int tx_word, rx_word, tmp;
	int txshift = priv->txshift;
	int rxshift = priv->rxshift;
	int txi, rxi, bits_per_char;
	int length;

	/* Use IOCTL for periodic reads. The FIFO is not supported in automated 
	 * periodic mode 
	 */
	if ( priv->periodic_cfg.periodic_mode ) {
		return -1;
	}

	bits_per_char = priv->bits_per_char;
	tx_word = 0;
	if ( !txbuf ) {
		tx_word = priv->idle_char << txshift;
	}

	/* Clear the events */
	priv->regs->event = 0xffffffff;

	/* Enable core */
	priv->regs->mode |= SPICTRL_MODE_EN | SPICTRL_MODE_MS;

	length = len;
	if ( bits_per_char > 8 ) {
		length = length / 2;
		if ( bits_per_char > 16 )
			length = length / 2;
	}
	DBG("SPICTRL: LENGTH = %d, Bits/Char: %d, Shift: %d, %d\n", length, bits_per_char, txshift, rxshift);

	txi=0;
	rxi=0;
	while ( (rxi < length) || (txi < length) ) {
		/* Get transmit word */
		if ( length > txi ) {
			if ( txbuf ) {
				if ( bits_per_char <= 8 ) {
					tx_word = *((unsigned char *)txbuf + txi);
				} else if ( bits_per_char <= 16 ) {
					tx_word = *((unsigned short *)txbuf + txi);
				} else {
					tx_word = *((unsigned int *)txbuf + txi);
				}
				tx_word = tx_word << txshift;
			}

			/* Wait for SPICTRL to get ready for another TX char */
			while ( (priv->regs->event & SPICTRL_EVENT_NF) == 0 ) {
				/* Wait for all chars to transmit */
/* Could implement waiting for SPICTRL IRQ here */
			}

			DBG("SPICTRL: Writing 0x%x\n", tx_word);

			/* Transmit word */
			priv->regs->tx = tx_word;
			txi++;
		}

		/* Read */
		while ( priv->regs->event & SPICTRL_EVENT_NE ) {
			/* Read to avoid overrun */
			tmp = priv->regs->rx;
			DBG("SPICTRL: Read 0x%x\n", tmp);

			if ( rxbuf && (length > rxi) ) {
				/* Copy word to user buffer */
				rx_word = (tmp >> rxshift);

				DBG("SPICTRL: Receiving 0x%x (0x%x, %d)\n", rx_word, tmp, rxshift);

				if ( bits_per_char <= 8 ) {
					*((unsigned char *)rxbuf + rxi) = rx_word;
				} else if ( bits_per_char <= 16 ) {
					*((unsigned short *)rxbuf + rxi) = rx_word;
				} else {
					*((unsigned int *)rxbuf + rxi) = rx_word;
				}

			}
			rxi++;
		}
	}

	return len;
}


STATIC rtems_status_code spictrl_libi2c_init(rtems_libi2c_bus_t *bushdl)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;

	DBG("SPICTRL: spictrl_libi2c_init\n");

	/* Disable SPICTTRL, Select Master mode */
	priv->regs->mode = SPICTRL_MODE_MS;

	/* Mask all Interrupts */
	priv->regs->mask = 0;

	/* Select no slave */
	priv->regs->slvsel = 0xffffffff;

	/* Clear all events */
	priv->regs->event = 0xffffffff;

	return 0;
}

/* Nothing to be done in start */
STATIC rtems_status_code spictrl_libi2c_send_start(rtems_libi2c_bus_t *bushdl)
{
	DBG("SPICTRL: spictrl_libi2c_send_start\n");

	return 0;
}

/* Inactivate all chip selects, indicates "End of command" */
STATIC rtems_status_code spictrl_libi2c_send_stop(rtems_libi2c_bus_t *bushdl)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;

	priv->regs->slvsel = 0xffffffff;

	if ( priv->slvSelFunc ) {
		/* unslect all */
		return priv->slvSelFunc(priv->regs, -1, 0);
	}

	DBG("SPICTRL: spictrl_libi2c_send_stop\n");
	return 0;
}

/* Select Slave address by selecting apropriate chip select */
STATIC rtems_status_code spictrl_libi2c_send_addr(rtems_libi2c_bus_t *bushdl,
					     uint32_t addr, int rw)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;

	DBG("SPICTRL: spictrl_libi2c_send_addr, %d\n", addr);

	if ( priv->slvSelFunc ) {
		/* Let user set spi select using for example GPIO */
		return priv->slvSelFunc(priv->regs, addr, 1);
	} else if ( priv->regs->capability & SPICTRL_CAP_SSEN ) {
		int slaves;

		/* Maximum number of slaves the core support */
		slaves = (priv->regs->capability & SPICTRL_CAP_SSSZ) >> SPICTRL_CAP_SSSZ_BIT;

		if ( addr > slaves )
			return -1;

		if ( (priv->regs->capability & SPICTRL_CAP_ASELA) &&
		     (priv->periodic_cfg.period_flags & SPICTRL_PERIOD_FLAGS_ASEL) ) {
		     	/* When automatic slave select is supported by hardware and
			 * enabled by configuration the SPI address is determined by 
			 * the automatic slave select register and the "idle" slave
			 * select register is set by configuration.
			 */
			priv->regs->am_slvsel = ~(1<<(addr-1));
			priv->regs->slvsel = priv->periodic_cfg.period_slvsel;
			/* Enable automatic slave select */
			priv->regs->mode |= SPICTRL_MODE_ASEL;
		} else {
			/* Normal mode */
			priv->regs->slvsel = ~(1<<(addr-1));
		}
	}

	return 0;
}

/* Read a number of bytes */
STATIC int spictrl_libi2c_read_bytes(rtems_libi2c_bus_t *bushdl, 
				unsigned char *bytes, int nbytes)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;
	int ret;

	DBG("SPICTRL: spictrl_libi2c_read_bytes %d\n", nbytes);
	ret = spictrl_read_write(priv, bytes, NULL, nbytes);
	if ( ret < 0 ) {
		printk("SPICTRL: Error Reading\n");
	} 
#ifdef DEBUG
	else {
		int i;
		for(i=0; i<nbytes; i+=16) {
			DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", 
				bytes[0+i], bytes[1+i], bytes[2+i], bytes[3+i], bytes[4+i], bytes[5+i], bytes[6+i], bytes[7+i]);
			DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
				bytes[8+i], bytes[9+i], bytes[10+i], bytes[11+i], bytes[12+i], bytes[13+i], bytes[14+i], bytes[15+i]);
		}
	}
#endif
	return ret;
}

/* Write a number of bytes */
STATIC int spictrl_libi2c_write_bytes(rtems_libi2c_bus_t *bushdl, 
				unsigned char *bytes, int nbytes)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;

#ifdef DEBUG
	int i;
	DBG("SPICTRL: spictrl_libi2c_write_bytes: %d\n", nbytes);

	for(i=0; i<nbytes; i+=16) {
		DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", 
			bytes[0+i], bytes[1+i], bytes[2+i], bytes[3+i], bytes[4+i], bytes[5+i], bytes[6+i], bytes[7+i]);
		DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
			bytes[8+i], bytes[9+i], bytes[10+i], bytes[11+i], bytes[12+i], bytes[13+i], bytes[14+i], bytes[15+i]);
	}
#endif

	return spictrl_read_write(priv, NULL, bytes, nbytes);
}

/* Configure the interface and do simultaneous READ/WRITE operations */
STATIC int spictrl_libi2c_ioctl(
	rtems_libi2c_bus_t * bushdl, 
	int   cmd,
	void *buffer)
{
	struct spictrl_priv *priv = (struct spictrl_priv *)bushdl;
	int ret;

	DBG("SPICTRL: spictrl_libi2c_ioctl(%d, 0x%x)\n", cmd, (unsigned int)buffer);

	switch (cmd) {
		case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
		{
			rtems_libi2c_tfr_mode_t *trf_mode = buffer;
			unsigned int mode;

			/* Must disable core to write new values */
			priv->regs->mode &= ~SPICTRL_MODE_EN;

			/* Change bit frequency */
			if ( spictrl_set_freq(priv, trf_mode->baudrate) ) {
				/* Unable to set such a low frequency. */
				return -1;
			}

			/* Set Clock Polarity, Clock Phase, Reverse mode and Word Length */
			mode = (priv->regs->mode & 
				~(SPICTRL_MODE_CPOL|SPICTRL_MODE_CPHA|SPICTRL_MODE_REV|SPICTRL_MODE_LEN));
			if ( trf_mode->clock_inv ) 
				mode |= SPICTRL_MODE_CPOL;
			if ( trf_mode->clock_phs ) 
				mode |= SPICTRL_MODE_CPHA;
			if ( trf_mode->lsb_first == 0 )
				mode |= SPICTRL_MODE_REV; /* Set Reverse mode (MSB first) */

			if ( (trf_mode->bits_per_char < 4) || 
				((trf_mode->bits_per_char > 16) && (trf_mode->bits_per_char != 32)) )
				return -1;
			if ( trf_mode->bits_per_char == 32 ) {
				priv->txshift = 0;
				priv->rxshift = 0;
			} else {
				mode |= (trf_mode->bits_per_char-1) << SPICTRL_MODE_LEN_BIT;
				if ( trf_mode->lsb_first == 0 ) {
					/* REV bit 1 */
					priv->txshift = 32 - trf_mode->bits_per_char;
					priv->rxshift = 16;
				} else {
					/* REV bit 0 */
					priv->txshift = 0;
					priv->rxshift = 16 - trf_mode->bits_per_char;
				}
			}

			priv->bits_per_char = trf_mode->bits_per_char;
			priv->lsb_first = trf_mode->lsb_first;
			priv->idle_char = trf_mode->idle_char;

			/* Update hardware */
			priv->regs->mode = mode;

			return 0;
		}

		case RTEMS_LIBI2C_IOCTL_READ_WRITE:
		{
			rtems_libi2c_read_write_t *arg = buffer;

			DBG("SPICTRL: IOCTL READ/WRITE, RX: 0x%x, TX: 0x%x, len: %d\n", arg->rd_buf, arg->wr_buf, arg->byte_cnt);
#ifdef DEBUG
			/* Printf out what is going to be transmitted */
			if ( arg->wr_buf ) {
				unsigned char *bytes = (unsigned char *)arg->wr_buf;
				int i;
				for(i=0; i<arg->byte_cnt; i+=16) {
					DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", 
						bytes[0+i], bytes[1+i], bytes[2+i], bytes[3+i], bytes[4+i], bytes[5+i], bytes[6+i], bytes[7+i]);
					DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
						bytes[8+i], bytes[9+i], bytes[10+i], bytes[11+i], bytes[12+i], bytes[13+i], bytes[14+i], bytes[15+i]);
				}
			}
#endif

			ret = spictrl_read_write(priv, arg->rd_buf, (unsigned char *)arg->wr_buf,
				arg->byte_cnt);
#ifdef DEBUG
			/* Printf out what was read */
			if ( arg->rd_buf ) {
				unsigned char *bytes = (unsigned char *)arg->rd_buf;
				int i;
				for(i=0; i<arg->byte_cnt; i+=16) {
					DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", 
						bytes[0+i], bytes[1+i], bytes[2+i], bytes[3+i], bytes[4+i], bytes[5+i], bytes[6+i], bytes[7+i]);
					DBG("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
						bytes[8+i], bytes[9+i], bytes[10+i], bytes[11+i], bytes[12+i], bytes[13+i], bytes[14+i], bytes[15+i]);
				}
			}
#endif
			return ret;
		}

		/* Enable Periodic mode */
		case SPICTRL_IOCTL_CONFIG:
		{
			struct spictrl_ioctl_config *cfg;

			DBG("SPICTRL: Configuring Periodic mode\n");

			if ( priv->periodic_started ) {
				DBG("SPICTRL: Periodic mode already started, too late to configure\n");
				return -1;
			}

			cfg = buffer;
			if ( cfg == NULL ) {
				memset(&priv->periodic_cfg, 0, sizeof(priv->periodic_cfg));
			} else {
				priv->periodic_cfg = *cfg;
			}
			cfg = &priv->periodic_cfg;
			if ( cfg->periodic_mode ) {
				/* Enable Automated Periodic mode */
				priv->regs->mode |= SPICTRL_MODE_AMEN;

				/* Check that hardware has support for periodic mode */
				if ( (priv->regs->mode & SPICTRL_MODE_AMEN) == 0 ) {
					priv->periodic_cfg.periodic_mode = 0;
					DBG("SPICTRL: Periodic mode not supported by hardware\n");
					return -1;
				}
			} else {
				/* Disable Periodic mode */
				priv->regs->mode &= ~SPICTRL_MODE_AMEN;
			}
			priv->periodic_started = 0;

			/* Set clockgap and TAC */
			priv->regs->mode = (priv->regs->mode & ~(SPICTRL_MODE_CG|SPICTRL_MODE_TAC)) |
			                   (cfg->clock_gap << SPICTRL_MODE_CG_BIT) |
					   (cfg->flags & SPICTRL_MODE_TAC);
			return 0;
		}
		case SPICTRL_IOCTL_PERIOD_START:
		{
			if ( !priv->periodic_cfg.periodic_mode || priv->periodic_started ) {
				return -1;
			}
			if ( spictrl_start_periodic(priv) == 0 ) {
				priv->periodic_started = 1;
				return 0;
			} else
				return -1;
		}
		case SPICTRL_IOCTL_PERIOD_STOP:
		{
			if ( !priv->periodic_cfg.periodic_mode || !priv->periodic_started ) {
				return -1;
			}
			spictrl_stop_periodic(priv);
			priv->periodic_started = 0;
			return 0;
		}
		case SPICTRL_IOCTL_STATUS:
		{
			if ( !buffer )
				return 0;
			*(unsigned int *)buffer = spictrl_status(priv);
			return 0;
		}

		case SPICTRL_IOCTL_PERIOD_WRITE:
		{
			if ( !priv->periodic_cfg.periodic_mode || !buffer ) {
				return -1;
			}
			if ( spictrl_write_periodic(priv, (struct spictrl_period_io *)
			                            buffer) == 0 ) {
				return 0;
			} else
				return -1;
		}

		case SPICTRL_IOCTL_PERIOD_READ:
		{
			if ( !priv->periodic_cfg.periodic_mode || !buffer ) {
				return -1;
			}
			if ( spictrl_read_periodic(priv, (struct spictrl_period_io *)
			                            buffer) == 0 ) {
				return 0;
			} else
				return -1;
		}

		case SPICTRL_IOCTL_REGS:
		{
			/* Copy Register Base Address to user space */
			if ( !buffer ) {
				return -1;
			}
			*(struct spictrl_regs **)buffer = priv->regs;
			return 0;
		}

		default:
			/* Unknown IOCTL */
			return -1;
	}

	return 0;
}

STATIC rtems_libi2c_bus_ops_t spictrl_libi2c_ops =
{
	.init		= spictrl_libi2c_init,
	.send_start	= spictrl_libi2c_send_start,
	.send_stop	= spictrl_libi2c_send_stop,
	.send_addr	= spictrl_libi2c_send_addr,
	.read_bytes	= spictrl_libi2c_read_bytes,
	.write_bytes	= spictrl_libi2c_write_bytes,
	.ioctl		= spictrl_libi2c_ioctl
};

int spictrl_device_init(struct spictrl_priv *priv)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	priv->irq = pnpinfo->irq;
	priv->regs = (struct spictrl_regs *)pnpinfo->apb_slv->start;
	priv->fdepth = (priv->regs->capability & SPICTRL_CAP_FDEPTH) >> SPICTRL_CAP_FDEPTH_BIT;

	DBG("SPCTRL: 0x%x irq %d, FIFO: %d\n", (unsigned int)priv->regs, priv->irq, priv->fdepth);

	/* Mask all Interrupts */
	priv->regs->mask = 0;

	/* Disable SPICTTRL */
	priv->regs->mode = 0;

	/* Get custom */
	value = drvmgr_dev_key_get(priv->dev, "slvSelFunc", DRVMGR_KT_POINTER);
	if ( value ) {
		priv->slvSelFunc = value->ptr;
	}

	/* Prepare I2C layer */
	priv->i2clib_desc.ops = &spictrl_libi2c_ops;
	priv->i2clib_desc.size = sizeof(spictrl_libi2c_ops);
	return 0;
}
