/*
 *  GRCAN driver
 *
 *  COPYRIGHT (c) 2007.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <grlib/grcan.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>

#include <grlib/grlib_impl.h>

/* Maximum number of GRCAN devices supported by driver */
#define GRCAN_COUNT_MAX 8

#define WRAP_AROUND_TX_MSGS 1
#define WRAP_AROUND_RX_MSGS 2
#define GRCAN_MSG_SIZE sizeof(struct grcan_msg)
#define BLOCK_SIZE (16*4)

/* grcan needs to have it buffers aligned to 1k boundaries */
#define BUFFER_ALIGNMENT_NEEDS 1024

/* Default Maximium buffer size for statically allocated buffers */
#ifndef TX_BUF_SIZE
 #define TX_BUF_SIZE (BLOCK_SIZE*16)
#endif

/* Make receiver buffers bigger than transmitt */
#ifndef RX_BUF_SIZE
 #define RX_BUF_SIZE ((3*BLOCK_SIZE)*16)
#endif

#ifndef IRQ_CLEAR_PENDING
 #define IRQ_CLEAR_PENDING(irqno)
#endif

#ifndef IRQ_UNMASK
 #define IRQ_UNMASK(irqno)
#endif

#ifndef IRQ_MASK
 #define IRQ_MASK(irqno)
#endif

#ifndef GRCAN_DEFAULT_BAUD
 /* default to 500kbits/s */
 #define GRCAN_DEFAULT_BAUD 500000
#endif

#ifndef GRCAN_SAMPLING_POINT
 #define GRCAN_SAMPLING_POINT 80
#endif

/* Uncomment for debug output */
/****************** DEBUG Definitions ********************/
#define DBG_TX 2
#define DBG_RX 4
#define DBG_STATE 8

#define DEBUG_FLAGS (DBG_STATE | DBG_RX | DBG_TX )
/*
#define DEBUG
#define DEBUGFUNCS
*/
#include <grlib/debug_defs.h>

/*********************************************************/

int state2err[4] = {
	/* STATE_STOPPED */ GRCAN_RET_NOTSTARTED,
	/* STATE_STARTED */ GRCAN_RET_OK,
	/* STATE_BUSOFF  */ GRCAN_RET_BUSOFF,
	/* STATE_AHBERR  */ GRCAN_RET_AHBERR
};

struct grcan_msg {
	unsigned int head[2];
	unsigned char data[8];
};

struct grcan_config {
	struct grcan_timing timing;
	struct grcan_selection selection;
	int abort;
	int silent;
};

struct grcan_priv {
	struct drvmgr_dev *dev;	/* Driver manager device */
	char devName[32];	/* Device Name */
	unsigned int baseaddr, ram_base;
	struct grcan_regs *regs;
	int irq;
	int minor;
	int open;
	int started;
	unsigned int channel;
	int flushing;
	unsigned int corefreq_hz;

	/* Circular DMA buffers */
	void *_rx, *_rx_hw;
	void *_tx, *_tx_hw;
	void *txbuf_adr;
	void *rxbuf_adr;
	struct grcan_msg *rx;
	struct grcan_msg *tx;
	unsigned int rxbuf_size;    /* requested RX buf size in bytes */
	unsigned int txbuf_size;    /* requested TX buf size in bytes */

	int txblock, rxblock;
	int txcomplete, rxcomplete;

	struct grcan_filter sfilter;
	struct grcan_filter afilter;
	int config_changed; /* 0=no changes, 1=changes ==> a Core reset is needed */
	struct grcan_config config;
	struct grcan_stats stats;

	rtems_id rx_sem, tx_sem, txempty_sem, dev_sem;
	SPIN_DECLARE(devlock);
};

static void __inline__ grcan_hw_reset(struct grcan_regs *regs);

static int grcan_hw_read_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg *buffer,
	int max);

static int grcan_hw_write_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg *buffer,
	int count);

static void grcan_hw_config(
	struct grcan_regs *regs,
	struct grcan_config *conf);

static void grcan_hw_accept(
	struct grcan_regs *regs,
	struct grcan_filter *afilter);

static void grcan_hw_sync(
	struct grcan_regs *regs,
	struct grcan_filter *sfilter);

static void grcan_interrupt(void *arg);

#ifdef GRCAN_REG_BYPASS_CACHE
#define READ_REG(address) grlib_read_uncached32((unsigned int)(address))
#else
#define READ_REG(address) (*(volatile unsigned int *)(address))
#endif

#ifdef GRCAN_DMA_BYPASS_CACHE
#define READ_DMA_WORD(address) grlib_read_uncached32((unsigned int)(address))
#define READ_DMA_BYTE(address) grlib_read_uncached8((unsigned int)(address))
#else
#define READ_DMA_WORD(address) (*(volatile unsigned int *)(address))
#define READ_DMA_BYTE(address) (*(volatile unsigned char *)(address))
#endif

#define NELEM(a) ((int) (sizeof (a) / sizeof (a[0])))

static int grcan_count = 0;
static struct grcan_priv *priv_tab[GRCAN_COUNT_MAX];

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int grcan_device_init(struct grcan_priv *pDev);

int grcan_init2(struct drvmgr_dev *dev);
int grcan_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops grcan_ops = 
{
	.init = {NULL, grcan_init2, grcan_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id grcan_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRCAN},
	{VENDOR_GAISLER, GAISLER_GRHCAN},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info grcan_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRCAN_ID,	/* Driver ID */
		"GRCAN_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grcan_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&grcan_ids[0]
};

void grcan_register_drv (void)
{
	DBG("Registering GRCAN driver\n");
	drvmgr_drv_register(&grcan_drv_info.general);
}

int grcan_init2(struct drvmgr_dev *dev)
{
	struct grcan_priv *priv;

	DBG("GRCAN[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	if (GRCAN_COUNT_MAX <= grcan_count)
		return DRVMGR_ENORES;
	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int grcan_init3(struct drvmgr_dev *dev)
{
	struct grcan_priv *priv;
	char prefix[32];

	priv = dev->priv;

	/*
	 * Now we take care of device initialization.
	 */

	if ( grcan_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	priv_tab[grcan_count] = priv;
	grcan_count++;

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "grcan%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "%sgrcan%d", prefix, dev->minor_bus);
	}

	return DRVMGR_OK;
}

int grcan_device_init(struct grcan_priv *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irq = pnpinfo->irq;
	pDev->regs = (struct grcan_regs *)pnpinfo->apb_slv->start;
	pDev->minor = pDev->dev->minor_drv;

	/* Get frequency in Hz */
	if ( drvmgr_freq_get(pDev->dev, DEV_APB_SLV, &pDev->corefreq_hz) ) {
		return -1;
	}

	DBG("GRCAN frequency: %d Hz\n", pDev->corefreq_hz);

	/* Reset Hardware before attaching IRQ handler */
	grcan_hw_reset(pDev->regs);

	/* RX Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'R', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->rx_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	/* TX Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'T', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->tx_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	/* TX Empty Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'E', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->txempty_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'A', '0' + pDev->minor),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&pDev->dev_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	return 0;
}

static void __inline__ grcan_hw_reset(struct grcan_regs *regs)
{
	regs->ctrl = GRCAN_CTRL_RESET;
}

static rtems_device_driver grcan_hw_start(struct grcan_priv *pDev)
{
	/*
	 * tmp is set but never used. GCC gives a warning for this
	 * and we need to tell GCC not to complain.
	 */
	unsigned int tmp RTEMS_UNUSED;

	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	/* Check that memory has been allocated successfully */
	if (!pDev->tx || !pDev->rx)
		return RTEMS_NO_MEMORY;

	/* Configure FIFO configuration register
	 * and Setup timing
	 */
	if (pDev->config_changed) {
		grcan_hw_config(pDev->regs, &pDev->config);
		pDev->config_changed = 0;
	}

	/* Setup receiver */
	pDev->regs->rx0addr = (unsigned int)pDev->_rx_hw;
	pDev->regs->rx0size = pDev->rxbuf_size;

	/* Setup Transmitter */
	pDev->regs->tx0addr = (unsigned int)pDev->_tx_hw;
	pDev->regs->tx0size = pDev->txbuf_size;

	/* Setup acceptance filters */
	grcan_hw_accept(pDev->regs, &pDev->afilter);

	/* Sync filters */
	grcan_hw_sync(pDev->regs, &pDev->sfilter);

	/* Clear status bits */
	tmp = READ_REG(&pDev->regs->stat);
	pDev->regs->stat = 0;

	/* Setup IRQ handling */

	/* Clear all IRQs */
	tmp = READ_REG(&pDev->regs->pir);
	pDev->regs->picr = 0x1ffff;

	/* unmask TxLoss|TxErrCntr|RxErrCntr|TxAHBErr|RxAHBErr|OR|OFF|PASS */
	pDev->regs->imr = 0x1601f;

	/* Enable routing of the IRQs */
	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	IRQ_UNMASK(pDev->irq + GRCAN_IRQ_TXSYNC);
	IRQ_UNMASK(pDev->irq + GRCAN_IRQ_RXSYNC);
	IRQ_UNMASK(pDev->irq + GRCAN_IRQ_IRQ);
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	/* Enable receiver/transmitter */
	pDev->regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
	pDev->regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;

	/* Enable HurriCANe core */
	pDev->regs->ctrl = GRCAN_CTRL_ENABLE;

	/* Leave transmitter disabled, it is enabled when
	 * trying to send something.
	 */
	return RTEMS_SUCCESSFUL;
}

static void grcan_hw_stop(struct grcan_priv *pDev)
{
	FUNCDBG();

	/* Mask all IRQs */
	pDev->regs->imr = 0;
	IRQ_MASK(pDev->irq + GRCAN_IRQ_TXSYNC);
	IRQ_MASK(pDev->irq + GRCAN_IRQ_RXSYNC);
	IRQ_MASK(pDev->irq + GRCAN_IRQ_IRQ);

	/* Disable receiver & transmitter */
	pDev->regs->rx0ctrl = 0;
	pDev->regs->tx0ctrl = 0;
}

static void grcan_sw_stop(struct grcan_priv *pDev)
{
	/*
	 * Release semaphores to wake all threads waiting for an IRQ.
	 * The threads that
	 * get woken up must check started state in
	 * order to determine that they should return to
	 * user space with error status.
	 *
	 * Entering into started mode again will reset the
	 * semaphore count.
	 */
	rtems_semaphore_release(pDev->rx_sem);
	rtems_semaphore_release(pDev->tx_sem);
	rtems_semaphore_release(pDev->txempty_sem);
}

static void grcan_hw_config(struct grcan_regs *regs, struct grcan_config *conf)
{
	unsigned int config = 0;

	/* Reset HurriCANe Core */
	regs->ctrl = 0;

	if (conf->silent)
		config |= GRCAN_CFG_SILENT;

	if (conf->abort)
		config |= GRCAN_CFG_ABORT;

	if (conf->selection.selection)
		config |= GRCAN_CFG_SELECTION;

	if (conf->selection.enable0)
		config |= GRCAN_CFG_ENABLE0;

	if (conf->selection.enable1)
		config |= GRCAN_CFG_ENABLE1;

	/* Timing */
	config |= (conf->timing.bpr << GRCAN_CFG_BPR_BIT) & GRCAN_CFG_BPR;
	config |= (conf->timing.rsj << GRCAN_CFG_RSJ_BIT) & GRCAN_CFG_RSJ;
	config |= (conf->timing.ps1 << GRCAN_CFG_PS1_BIT) & GRCAN_CFG_PS1;
	config |= (conf->timing.ps2 << GRCAN_CFG_PS2_BIT) & GRCAN_CFG_PS2;
	config |=
	    (conf->timing.scaler << GRCAN_CFG_SCALER_BIT) & GRCAN_CFG_SCALER;

	/* Write configuration */
	regs->conf = config;

	/* Enable HurriCANe Core */
	regs->ctrl = GRCAN_CTRL_ENABLE;
}

static void grcan_hw_accept(
	struct grcan_regs *regs,
	struct grcan_filter *afilter
)
{
	/* Disable Sync mask totaly (if we change scode or smask
	 * in an unfortunate way we may trigger a sync match)
	 */
	regs->rx0mask = 0xffffffff;

	/* Set Sync Filter in a controlled way */
	regs->rx0code = afilter->code;
	regs->rx0mask = afilter->mask;
}

static void grcan_hw_sync(struct grcan_regs *regs, struct grcan_filter *sfilter)
{
	/* Disable Sync mask totaly (if we change scode or smask
	 * in an unfortunate way we may trigger a sync match)
	 */
	regs->smask = 0xffffffff;

	/* Set Sync Filter in a controlled way */
	regs->scode = sfilter->code;
	regs->smask = sfilter->mask;
}

static unsigned int grcan_hw_rxavail(
	unsigned int rp,
	unsigned int wp, unsigned int size
)
{
	if (rp == wp) {
		/* read pointer and write pointer is equal only
		 * when RX buffer is empty.
		 */
		return 0;
	}

	if (wp > rp) {
		return (wp - rp) / GRCAN_MSG_SIZE;
	} else {
		return (size - (rp - wp)) / GRCAN_MSG_SIZE;
	}
}

static unsigned int grcan_hw_txspace(
	unsigned int rp,
	unsigned int wp,
	unsigned int size
)
{
	unsigned int left;

	if (rp == wp) {
		/* read pointer and write pointer is equal only
		 * when TX buffer is empty.
		 */
		return size / GRCAN_MSG_SIZE - WRAP_AROUND_TX_MSGS;
	}

	/* size - 4 - abs(read-write) */
	if (wp > rp) {
		left = size - (wp - rp);
	} else {
		left = rp - wp;
	}

	return left / GRCAN_MSG_SIZE - WRAP_AROUND_TX_MSGS;
}

#define MIN_TSEG1 1
#define MIN_TSEG2 2
#define MAX_TSEG1 14
#define MAX_TSEG2 8

static int grcan_calc_timing(
	unsigned int baud,	/* The requested BAUD to calculate timing for */
	unsigned int core_hz,	/* Frequency in Hz of GRCAN Core */
	unsigned int sampl_pt,
	struct grcan_timing *timing	/* result is placed here */
)
{
	int best_error = 1000000000;
	int error;
	int best_tseg = 0, best_brp = 0, brp = 0;
	int tseg = 0, tseg1 = 0, tseg2 = 0;
	int sjw = 1;

	/* Default to 90% */
	if ((sampl_pt < 50) || (sampl_pt > 99)) {
		sampl_pt = GRCAN_SAMPLING_POINT;
	}

	if ((baud < 5000) || (baud > 1000000)) {
		/* invalid speed mode */
		return -1;
	}

	/* find best match, return -2 if no good reg
	 * combination is available for this frequency
	 */

	/* some heuristic specials */
	if (baud > ((1000000 + 500000) / 2))
		sampl_pt = 75;

	if (baud < ((12500 + 10000) / 2))
		sampl_pt = 75;

	/* tseg even = round down, odd = round up */
	for (
		tseg = (MIN_TSEG1 + MIN_TSEG2 + 2) * 2;
		tseg <= (MAX_TSEG2 + MAX_TSEG1 + 2) * 2 + 1;
		tseg++
	) {
		brp = core_hz / ((1 + tseg / 2) * baud) + tseg % 2;
		if (
			(brp <= 0) ||
			((brp > 256 * 1) && (brp <= 256 * 2) && (brp & 0x1)) ||
			((brp > 256 * 2) && (brp <= 256 * 4) && (brp & 0x3)) ||
			((brp > 256 * 4) && (brp <= 256 * 8) && (brp & 0x7)) ||
			(brp > 256 * 8)
		)
			continue;

		error = baud - core_hz / (brp * (1 + tseg / 2));
		if (error < 0) {
			error = -error;
		}

		if (error <= best_error) {
			best_error = error;
			best_tseg = tseg / 2;
			best_brp = brp - 1;
		}
	}

	if (best_error && (baud / best_error < 10)) {
		return -2;
	} else if (!timing)
		return 0;	/* nothing to store result in, but a valid bitrate can be calculated */

	tseg2 = best_tseg - (sampl_pt * (best_tseg + 1)) / 100;

	if (tseg2 < MIN_TSEG2) {
		tseg2 = MIN_TSEG2;
	}

	if (tseg2 > MAX_TSEG2) {
		tseg2 = MAX_TSEG2;
	}

	tseg1 = best_tseg - tseg2 - 2;

	if (tseg1 > MAX_TSEG1) {
		tseg1 = MAX_TSEG1;
		tseg2 = best_tseg - tseg1 - 2;
	}

	/* Get scaler and BRP from pseudo BRP */
	if (best_brp <= 256) {
		timing->scaler = best_brp;
		timing->bpr = 0;
	} else if (best_brp <= 256 * 2) {
		timing->scaler = ((best_brp + 1) >> 1) - 1;
		timing->bpr = 1;
	} else if (best_brp <= 256 * 4) {
		timing->scaler = ((best_brp + 1) >> 2) - 1;
		timing->bpr = 2;
	} else {
		timing->scaler = ((best_brp + 1) >> 3) - 1;
		timing->bpr = 3;
	}

	timing->ps1 = tseg1 + 1;
	timing->ps2 = tseg2;
	timing->rsj = sjw;

	return 0;
}

static int grcan_hw_read_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg * buffer,
	int max
)
{
	int i, j;
	CANMsg *dest;
	struct grcan_msg *source, tmp;
	unsigned int wp, rp, size, rxmax, addr;
	int trunk_msg_cnt;

	FUNCDBG();

	wp = READ_REG(&regs->rx0wr);
	rp = READ_REG(&regs->rx0rd);

	/*
	 * Due to hardware wrap around simplification write pointer will
	 * never reach the read pointer, at least a gap of 8 bytes.
	 * The only time they are equal is when the read pointer has
	 * reached the write pointer (empty buffer)
	 *
	 */
	if (wp != rp) {
		/* Not empty, we have received chars...
		 * Read as much as possible from DMA buffer
		 */
		size = READ_REG(&regs->rx0size);

		/* Get number of bytes available in RX buffer */
		trunk_msg_cnt = grcan_hw_rxavail(rp, wp, size);

		/* truncate size if user space buffer hasn't room for
		 * all received chars.
		 */
		if (trunk_msg_cnt > max)
			trunk_msg_cnt = max;

		/* Read until i is 0 */
		i = trunk_msg_cnt;

		addr = (unsigned int)pDev->rx;
		source = (struct grcan_msg *)(addr + rp);
		dest = buffer;
		rxmax = addr + (size - GRCAN_MSG_SIZE);

		/* Read as many can messages as possible */
		while (i > 0) {
			/* Read CAN message from DMA buffer */
			tmp.head[0] = READ_DMA_WORD(&source->head[0]);
			tmp.head[1] = READ_DMA_WORD(&source->head[1]);
			if (tmp.head[1] & 0x4) {
				DBGC(DBG_RX, "overrun\n");
			}
			if (tmp.head[1] & 0x2) {
				DBGC(DBG_RX, "bus-off mode\n");
			}
			if (tmp.head[1] & 0x1) {
				DBGC(DBG_RX, "error-passive mode\n");
			}
			/* Convert one grcan CAN message to one "software" CAN message */
			dest->extended = tmp.head[0] >> 31;
			dest->rtr = (tmp.head[0] >> 30) & 0x1;
			if (dest->extended) {
				dest->id = tmp.head[0] & 0x3fffffff;
			} else {
				dest->id = (tmp.head[0] >> 18) & 0xfff;
			}
			dest->len = tmp.head[1] >> 28;
			for (j = 0; j < dest->len; j++)
				dest->data[j] = READ_DMA_BYTE(&source->data[j]);

			/* wrap around if neccessary */
			source =
			    ((unsigned int)source >= rxmax) ?
			    (struct grcan_msg *)addr : source + 1;
			dest++;	/* straight user buffer */
			i--;
		}
		{
			/* A bus off interrupt may have occured after checking pDev->started */
			SPIN_IRQFLAGS(oldLevel);

			SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
			if (pDev->started == STATE_STARTED) {
				regs->rx0rd = (unsigned int) source - addr;
				regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
			} else {
				DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
				trunk_msg_cnt = state2err[pDev->started];
			}
			SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		}
		return trunk_msg_cnt;
	}
	return 0;
}

static int grcan_hw_write_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg * buffer,
	int count
)
{
	unsigned int rp, wp, size, txmax, addr;
	int ret;
	struct grcan_msg *dest;
	CANMsg *source;
	int space_left;
	unsigned int tmp;
	int i;

	DBGC(DBG_TX, "\n");
	/*FUNCDBG(); */

	rp = READ_REG(&regs->tx0rd);
	wp = READ_REG(&regs->tx0wr);
	size = READ_REG(&regs->tx0size);

	space_left = grcan_hw_txspace(rp, wp, size);

	/* is circular fifo full? */
	if (space_left < 1)
		return 0;

	/* Truncate size */
	if (space_left > count)
		space_left = count;
	ret = space_left;

	addr = (unsigned int)pDev->tx;

	dest = (struct grcan_msg *)(addr + wp);
	source = (CANMsg *) buffer;
	txmax = addr + (size - GRCAN_MSG_SIZE);

	while (space_left > 0) {
		/* Convert and write CAN message to DMA buffer */
		if (source->extended) {
			tmp = (1 << 31) | (source->id & 0x3fffffff);
		} else {
			tmp = (source->id & 0xfff) << 18;
		}
		if (source->rtr)
			tmp |= (1 << 30);
		dest->head[0] = tmp;
		dest->head[1] = source->len << 28;
		for (i = 0; i < source->len; i++)
			dest->data[i] = source->data[i];
		source++;	/* straight user buffer */
		dest =
		    ((unsigned int)dest >= txmax) ?
		    (struct grcan_msg *)addr : dest + 1;
		space_left--;
	}

	{
		/* A bus off interrupt may have occured after checking pDev->started */
		SPIN_IRQFLAGS(oldLevel);

		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		if (pDev->started == STATE_STARTED) {
			regs->tx0wr = (unsigned int) dest - addr;
			regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
		} else {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			ret = state2err[pDev->started];
		}
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}
	return ret;
}

static int grcan_wait_rxdata(struct grcan_priv *pDev, int min)
{
	unsigned int wp, rp, size, irq;
	unsigned int irq_trunk, dataavail;
	int wait, state;
	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	/*** block until receive IRQ received
	 * Set up a valid IRQ point so that an IRQ is received
	 * when one or more messages are received
	 */
	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	state = pDev->started;

	/* A bus off interrupt may have occured after checking pDev->started */
	if (state != STATE_STARTED) {
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		if (state == STATE_BUSOFF) {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
		} else if (state == STATE_AHBERR) {
			DBGC(DBG_STATE, "cancelled due to a AHB error\n");
		} else {
			DBGC(DBG_STATE, "cancelled due to STOP (unexpected) \n");
		}
		return state2err[state];
	}

	size = READ_REG(&pDev->regs->rx0size);
	rp = READ_REG(&pDev->regs->rx0rd);
	wp = READ_REG(&pDev->regs->rx0wr);

	/**** Calculate IRQ Pointer ****/
	irq = wp + min * GRCAN_MSG_SIZE;
	/* wrap irq around */
	if (irq >= size) {
		irq_trunk = irq - size;
	} else
		irq_trunk = irq;

	/* init IRQ HW */
	pDev->regs->rx0irq = irq_trunk;

	/* Clear pending Rx IRQ */
	pDev->regs->picr = GRCAN_RXIRQ_IRQ;

	wp = READ_REG(&pDev->regs->rx0wr);

	/* Calculate messages available */
	dataavail = grcan_hw_rxavail(rp, wp, size);

	if (dataavail < min) {
		/* Still empty, proceed with sleep - Turn on IRQ (unmask irq) */
		pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_RXIRQ_IRQ;
		wait = 1;
	} else {
		/* enough message has been received, abort sleep - don't unmask interrupt */
		wait = 0;
	}
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	/* Wait for IRQ to fire only if has been triggered */
	if (wait) {
		rtems_semaphore_obtain(pDev->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		/*
		 * The semaphore is released either due to the expected IRQ
		 * condition or by BUSOFF, AHBERROR or another thread calling
		 * grcan_stop(). In either case, state2err[] has the correnct
		 * return value.
		 */
		return state2err[pDev->started];
	}

	return 0;
}

/* Wait for TX circular buffer to have room for min CAN messagges. TXIRQ is used to pin
 * point the location of the CAN message corresponding to min.
 *
 * min must be at least WRAP_AROUND_TX_MSGS less than max buffer capacity
 * (pDev->txbuf_size/GRCAN_MSG_SIZE) for this algo to work.
 */
static int grcan_wait_txspace(struct grcan_priv *pDev, int min)
{
	int wait, state;
	unsigned int irq, rp, wp, size, space_left;
	unsigned int irq_trunk;
	SPIN_IRQFLAGS(oldLevel);

	DBGC(DBG_TX, "\n");
	/*FUNCDBG(); */

	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	state = pDev->started;
	/* A bus off interrupt may have occured after checking pDev->started */
	if (state != STATE_STARTED) {
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		if (state == STATE_BUSOFF) {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
		} else if (state == STATE_AHBERR) {
			DBGC(DBG_STATE, "cancelled due to a AHB error\n");
		} else {
			DBGC(DBG_STATE, "cancelled due to STOP (unexpected)\n");
		}
		return state2err[state];
	}

	pDev->regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;

	size = READ_REG(&pDev->regs->tx0size);
	wp = READ_REG(&pDev->regs->tx0wr);

	rp = READ_REG(&pDev->regs->tx0rd);

	/**** Calculate IRQ Pointer ****/
	irq = rp + min * GRCAN_MSG_SIZE;
	/* wrap irq around */
	if (irq >= size) {
		irq_trunk = irq - size;
	} else
		irq_trunk = irq;

	/* trigger HW to do a IRQ when enough room in buffer */
	pDev->regs->tx0irq = irq_trunk;

	/* Clear pending Tx IRQ */
	pDev->regs->picr = GRCAN_TXIRQ_IRQ;

	/* One problem, if HW already gone past IRQ place the IRQ will
	 * never be received resulting in a thread hang. We check if so
	 * before proceeding.
	 *
	 * has the HW already gone past the IRQ generation place?
	 *  == does min fit info tx buffer?
	 */
	rp = READ_REG(&pDev->regs->tx0rd);

	space_left = grcan_hw_txspace(rp, wp, size);

	if (space_left < min) {
		/* Still too full, proceed with sleep - Turn on IRQ (unmask irq) */
		pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_TXIRQ_IRQ;
		wait = 1;
	} else {
		/* There are enough room in buffer, abort wait - don't unmask interrupt */
		wait = 0;
	}
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	/* Wait for IRQ to fire only if it has been triggered */
	if (wait) {
		rtems_semaphore_obtain(pDev->tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		return state2err[pDev->started];
	}

	/* At this point the TxIRQ has been masked, we ned not to mask it */
	return 0;
}

static int grcan_tx_flush(struct grcan_priv *pDev)
{
	int wait, state;
	unsigned int rp, wp;
	SPIN_IRQFLAGS(oldLevel);
	FUNCDBG();

	/* loop until all data in circular buffer has been read by hw.
	 * (write pointer != read pointer )
	 *
	 * Hardware doesn't update write pointer - we do
	 */
	while (
		(wp = READ_REG(&pDev->regs->tx0wr)) !=
		(rp = READ_REG(&pDev->regs->tx0rd))
	) {
		/* Wait for TX empty IRQ */
		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		state = pDev->started;

		/* A bus off interrupt may have occured after checking pDev->started */
		if (state != STATE_STARTED) {
			SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
			if (state == STATE_BUSOFF) {
				DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			} else if (state == STATE_AHBERR) {
				DBGC(DBG_STATE, "cancelled due to a AHB error\n");
			} else {
				DBGC(DBG_STATE, "cancelled due to STOP (unexpected)\n");
			}
			return state2err[state];
		}

		/* Clear pending TXEmpty IRQ */
		pDev->regs->picr = GRCAN_TXEMPTY_IRQ;

		if (wp != READ_REG(&pDev->regs->tx0rd)) {
			/* Still not empty, proceed with sleep - Turn on IRQ (unmask irq) */
			pDev->regs->imr =
			    READ_REG(&pDev->regs->imr) | GRCAN_TXEMPTY_IRQ;
			wait = 1;
		} else {
			/* TX fifo is empty */
			wait = 0;
		}
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		if (!wait)
			break;

		/* Wait for IRQ to wake us */
		rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		state = pDev->started;
		if (state != STATE_STARTED) {
			return state2err[state];
		}
	}
	return 0;
}

static int grcan_alloc_buffers(struct grcan_priv *pDev, int rx, int tx)
{
	unsigned int adr;
	FUNCDBG();

	if ( tx ) {
		adr = (unsigned int)pDev->txbuf_adr;
		if (adr & 0x1) {
			/* User defined "remote" address. Translate it into
			 * a CPU accessible address
			 */
			pDev->_tx_hw = (void *)(adr & ~0x1);
			drvmgr_translate_check(
				pDev->dev,
				DMAMEM_TO_CPU,
				(void *)pDev->_tx_hw,
				(void **)&pDev->_tx,
				pDev->txbuf_size);
			pDev->tx = (struct grcan_msg *)pDev->_tx;
		} else {
			if (adr == 0) {
				pDev->_tx = grlib_malloc(pDev->txbuf_size +
				                   BUFFER_ALIGNMENT_NEEDS);
				if (!pDev->_tx)
					return -1;
			} else {
				/* User defined "cou-local" address. Translate
				 * it into a CPU accessible address
				 */
				pDev->_tx = (void *)adr;
			}
			/* Align TX buffer */
			pDev->tx = (struct grcan_msg *)
			           (((unsigned int)pDev->_tx +
				   (BUFFER_ALIGNMENT_NEEDS-1)) &
			           ~(BUFFER_ALIGNMENT_NEEDS-1));

			/* Translate address into an hardware accessible
			 * address
			 */
			drvmgr_translate_check(
				pDev->dev,
				CPUMEM_TO_DMA,
				(void *)pDev->tx,
				(void **)&pDev->_tx_hw,
				pDev->txbuf_size);
		}
	}

	if ( rx ) {
		adr = (unsigned int)pDev->rxbuf_adr;
		if (adr & 0x1) {
			/* User defined "remote" address. Translate it into
			 * a CPU accessible address
			 */
			pDev->_rx_hw = (void *)(adr & ~0x1);
			drvmgr_translate_check(
				pDev->dev,
				DMAMEM_TO_CPU,
				(void *)pDev->_rx_hw,
				(void **)&pDev->_rx,
				pDev->rxbuf_size);
			pDev->rx = (struct grcan_msg *)pDev->_rx;
		} else {
			if (adr == 0) {
				pDev->_rx = grlib_malloc(pDev->rxbuf_size +
				                   BUFFER_ALIGNMENT_NEEDS);
				if (!pDev->_rx)
					return -1;
			} else {
				/* User defined "cou-local" address. Translate
				 * it into a CPU accessible address
				 */
				pDev->_rx = (void *)adr;
			}
			/* Align RX buffer */
			pDev->rx = (struct grcan_msg *)
			           (((unsigned int)pDev->_rx +
				   (BUFFER_ALIGNMENT_NEEDS-1)) &
			           ~(BUFFER_ALIGNMENT_NEEDS-1));

			/* Translate address into an hardware accessible
			 * address
			 */
			drvmgr_translate_check(
				pDev->dev,
				CPUMEM_TO_DMA,
				(void *)pDev->rx,
				(void **)&pDev->_rx_hw,
				pDev->rxbuf_size);
		}
	}
	return 0;
}

static void grcan_free_buffers(struct grcan_priv *pDev, int rx, int tx)
{
	FUNCDBG();

	if (tx && pDev->_tx) {
		free(pDev->_tx);
		pDev->_tx = NULL;
		pDev->tx = NULL;
	}

	if (rx && pDev->_rx) {
		free(pDev->_rx);
		pDev->_rx = NULL;
		pDev->rx = NULL;
	}
}

int grcan_dev_count(void)
{
	return grcan_count;
}

void *grcan_open_by_name(char *name, int *dev_no)
{
	int i;
	for (i = 0; i < grcan_count; i++){
		struct grcan_priv *pDev;

		pDev = priv_tab[i];
		if (NULL == pDev) {
			continue;
		}
		if (strncmp(pDev->devName, name, NELEM(pDev->devName)) == 0) {
			if (dev_no)
				*dev_no = i;
			return grcan_open(i);
		}
	}
	return NULL;
}

void *grcan_open(int dev_no)
{
	struct grcan_priv *pDev;
	void *ret;
	union drvmgr_key_value *value;

	FUNCDBG();

	if (grcan_count == 0 || (grcan_count <= dev_no)) {
		return NULL;
	}

	pDev = priv_tab[dev_no];

	/* Wait until we get semaphore */
	if (rtems_semaphore_obtain(pDev->dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL) {
		return NULL;
	}

	/* is device busy/taken? */
	if ( pDev->open ) {
		ret = NULL;
		goto out;
	}

	SPIN_INIT(&pDev->devlock, pDev->devName);

	/* Mark device taken */
	pDev->open = 1;

	pDev->txblock = pDev->rxblock = 1;
	pDev->txcomplete = pDev->rxcomplete = 0;
	pDev->started = STATE_STOPPED;
	pDev->config_changed = 1;
	pDev->config.silent = 0;
	pDev->config.abort = 0;
	pDev->config.selection.selection = 0;
	pDev->config.selection.enable0 = 0;
	pDev->config.selection.enable1 = 1;
	pDev->flushing = 0;
	pDev->rx = pDev->_rx = NULL;
	pDev->tx = pDev->_tx = NULL;
	pDev->txbuf_adr = 0;
	pDev->rxbuf_adr = 0;
	pDev->txbuf_size = TX_BUF_SIZE;
	pDev->rxbuf_size = RX_BUF_SIZE;

	/* Override default buffer sizes if available from bus resource */
	value = drvmgr_dev_key_get(pDev->dev, "txBufSize", DRVMGR_KT_INT);
	if ( value )
		pDev->txbuf_size = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "rxBufSize", DRVMGR_KT_INT);
	if ( value )
		pDev->rxbuf_size = value->i;

	value = drvmgr_dev_key_get(pDev->dev, "txBufAdr", DRVMGR_KT_POINTER);
	if ( value )
		pDev->txbuf_adr = value->ptr;

	value = drvmgr_dev_key_get(pDev->dev, "rxBufAdr", DRVMGR_KT_POINTER);
	if ( value )
		pDev->rxbuf_adr = value->ptr;

	DBG("Defaulting to rxbufsize: %d, txbufsize: %d\n",RX_BUF_SIZE,TX_BUF_SIZE);

	/* Default to accept all messages */
	pDev->afilter.mask = 0x00000000;
	pDev->afilter.code = 0x00000000;

	/* Default to disable sync messages (only trigger when id is set to all ones) */
	pDev->sfilter.mask = 0xffffffff;
	pDev->sfilter.code = 0x00000000;

	/* Calculate default timing register values */
	grcan_calc_timing(GRCAN_DEFAULT_BAUD,pDev->corefreq_hz,GRCAN_SAMPLING_POINT,&pDev->config.timing);

	if ( grcan_alloc_buffers(pDev,1,1) ) {
		ret = NULL;
		goto out;
	}

	/* Clear statistics */
	memset(&pDev->stats,0,sizeof(struct grcan_stats));

	ret = pDev;
out:
	rtems_semaphore_release(pDev->dev_sem);
	return ret;
}

int grcan_close(void *d)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	grcan_stop(d);

	grcan_hw_reset(pDev->regs);

	grcan_free_buffers(pDev,1,1);

	/* Mark Device as closed */
	pDev->open = 0;

	return 0;
}

int grcan_read(void *d, CANMsg *msg, size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANMsg *dest;
	unsigned int count, left;
	int nread;
	int req_cnt;

	FUNCDBG();

	dest = msg;
	req_cnt = ucount;

	if ( (!dest) || (req_cnt<1) )
		return GRCAN_RET_INVARG;

	if (pDev->started != STATE_STARTED) {
		return GRCAN_RET_NOTSTARTED;
	}

	DBGC(DBG_RX, "grcan_read [%p]: buf: %p len: %u\n", d, msg, (unsigned int) ucount);

	nread = grcan_hw_read_try(pDev,pDev->regs,dest,req_cnt);
	if (nread < 0) {
		return nread;
	}
	count = nread;
	if ( !( pDev->rxblock && pDev->rxcomplete && (count!=req_cnt) ) ){
		if ( count > 0 ) {
			/* Successfully received messages (at least one) */
			return count;
		}

		/* nothing read, shall we block? */
		if ( !pDev->rxblock ) {
			/* non-blocking mode */
			return GRCAN_RET_TIMEOUT;
		}
	}

	while (count == 0 || (pDev->rxcomplete && (count!=req_cnt))) {
		if (!pDev->rxcomplete) {
			left = 1; /* return as soon as there is one message available */
		} else {
			left = req_cnt - count;     /* return as soon as all data are available */

			/* never wait for more than the half the maximum size of the receive buffer 
			 * Why? We need some time to copy buffer before to catch up with hw,
			 * otherwise we would have to copy everything when the data has been
			 * received.
			 */
			if (left > ((pDev->rxbuf_size/GRCAN_MSG_SIZE) / 2)){
				left = (pDev->rxbuf_size/GRCAN_MSG_SIZE) / 2;
			}
		}

		nread = grcan_wait_rxdata(pDev, left);
		if (nread) {
			/* The wait has been aborted, probably due to 
			 * the device driver has been closed by another
			 * thread or a bus-off. Return error code.
			 */
			return nread;
		}

		/* Try read bytes from circular buffer */
		nread = grcan_hw_read_try(
				pDev,
				pDev->regs,
				dest+count,
				req_cnt-count);

		if (nread < 0) {
			/* The read was aborted by bus-off. */
			return nread;
		}
		count += nread;
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */
	return count;
}

int grcan_write(void *d, CANMsg *msg, size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANMsg *source;
	unsigned int count, left;
	int nwritten;
	int req_cnt;

	DBGC(DBG_TX,"\n");

	if ((pDev->started != STATE_STARTED) || pDev->config.silent || pDev->flushing)
		return GRCAN_RET_NOTSTARTED;

	req_cnt = ucount;
	source = (CANMsg *) msg;

	/* check proper length and buffer pointer */
	if (( req_cnt < 1) || (source == NULL) ){
		return GRCAN_RET_INVARG;
	}

	nwritten = grcan_hw_write_try(pDev,pDev->regs,source,req_cnt);
	if (nwritten < 0) {
		return nwritten;
	}
	count = nwritten;
	if ( !(pDev->txblock && pDev->txcomplete && (count!=req_cnt)) ) {
		if ( count > 0 ) {
			/* Successfully transmitted chars (at least one char) */
			return count;
		}

		/* nothing written, shall we block? */
		if ( !pDev->txblock ) {
			/* non-blocking mode */
			return GRCAN_RET_TIMEOUT;
		}
	}

	/* if in txcomplete mode we need to transmit all chars */
	while((count == 0) || (pDev->txcomplete && (count!=req_cnt)) ){
		/*** block until room to fit all or as much of transmit buffer as possible
		 * IRQ comes. Set up a valid IRQ point so that an IRQ is received 
		 * when we can put a chunk of data into transmit fifo
		 */
		if ( !pDev->txcomplete ){
			left = 1; /* wait for anything to fit buffer */
		}else{
			left = req_cnt - count; /* wait for all data to fit in buffer */

			/* never wait for more than the half the maximum size of the transmit
			 * buffer 
			 * Why? We need some time to fill buffer before hw catches up.
			 */
			if ( left > ((pDev->txbuf_size/GRCAN_MSG_SIZE)/2) ){
				left = (pDev->txbuf_size/GRCAN_MSG_SIZE)/2;
			}
		}

		nwritten = grcan_wait_txspace(pDev,left);
		/* Wait until more room in transmit buffer */
		if ( nwritten ) {
			/* The wait has been aborted, probably due to 
			 * the device driver has been closed by another
			 * thread. To avoid deadlock we return directly
			 * with error status.
			 */
			return nwritten;
		}

		/* Try read bytes from circular buffer */
		nwritten = grcan_hw_write_try(
			pDev,
			pDev->regs,
			source+count,
			req_cnt-count);

		if (nwritten < 0) {
			/* Write was aborted by bus-off. */
			return nwritten;
		}
		count += nwritten;
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */

	return count;
}

int grcan_start(void *d)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if (grcan_get_state(d) == STATE_STARTED) {
		return -1;
	}

	if ( (grcan_hw_start(pDev)) != RTEMS_SUCCESSFUL ){
		return -2;
	}

	/* Clear semaphore state. This is to avoid effects from previous
	 * bus-off/stop where semahpores where flushed() but the count remained.
	 */
	rtems_semaphore_obtain(pDev->rx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->tx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_NO_WAIT, 0);

	/* Read and write are now open... */
	pDev->started = STATE_STARTED;
	DBGC(DBG_STATE, "STOPPED|BUSOFF|AHBERR->STARTED\n");

	/* Register interrupt routine and enable IRQ at IRQ ctrl */
	drvmgr_interrupt_register(pDev->dev, 0, pDev->devName,
					grcan_interrupt, pDev);

	return 0;
}

int grcan_stop(void *d)
{
	struct grcan_priv *pDev = d;
	SPIN_IRQFLAGS(oldLevel);
	int do_sw_stop;

	FUNCDBG();

	if (pDev->started == STATE_STOPPED)
		return -1;

	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	if (pDev->started == STATE_STARTED) {
		grcan_hw_stop(pDev);
		do_sw_stop = 1;
		DBGC(DBG_STATE, "STARTED->STOPPED\n");
	} else {
		/*
		 * started == STATE_[STOPPED|BUSOFF|AHBERR] so grcan_hw_stop()
		 * might already been called from ISR.
		 */
		DBGC(DBG_STATE, "[STOPPED|BUSOFF|AHBERR]->STOPPED\n");
		do_sw_stop = 0;
	}
	pDev->started = STATE_STOPPED;
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	if (do_sw_stop)
		grcan_sw_stop(pDev);

	/* Disable interrupts */
	drvmgr_interrupt_unregister(pDev->dev, 0, grcan_interrupt, pDev);

	return 0;
}

int grcan_get_state(void *d)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	return pDev->started;
}

int grcan_flush(void *d)
{
	struct grcan_priv *pDev = d;
	int tmp;

	FUNCDBG();

	if ((pDev->started != STATE_STARTED) || pDev->flushing || pDev->config.silent)
		return -1;

	pDev->flushing = 1;
	tmp = grcan_tx_flush(pDev);
	pDev->flushing = 0;
	if ( tmp ) {
		/* The wait has been aborted, probably due to
		 * the device driver has been closed by another
		 * thread.
		 */
		return -1;
	}

	return 0;
}

int grcan_set_silent(void* d, int silent)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.silent = silent;
	pDev->config_changed = 1;

	return 0;
}

int grcan_set_abort(void* d, int abort)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.abort = abort;
	/* This Configuration parameter doesn't need HurriCANe reset
	 * ==> no pDev->config_changed = 1;
	 */

	return 0;
}

int grcan_set_selection(void *d, const struct grcan_selection *selection)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

	if ( !selection )
		return -2;

	pDev->config.selection = *selection;
	pDev->config_changed = 1;

	return 0;
}

int grcan_set_rxblock(void *d, int block)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	pDev->rxblock = block;

	return 0;
}

int grcan_set_txblock(void *d, int block)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	pDev->txblock = block;

	return 0;
}

int grcan_set_txcomplete(void *d, int complete)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	pDev->txcomplete = complete;

	return 0;
}

int grcan_set_rxcomplete(void *d, int complete)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	pDev->rxcomplete = complete;

	return 0;
}

int grcan_get_stats(void *d, struct grcan_stats *stats)
{
	struct grcan_priv *pDev = d;
	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	if ( !stats )
		return -1;

	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	*stats = pDev->stats;
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	return 0;
}

int grcan_clr_stats(void *d)
{
	struct grcan_priv *pDev = d;
	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	memset(&pDev->stats,0,sizeof(struct grcan_stats));
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	return 0;
}

int grcan_set_speed(void *d, unsigned int speed)
{
	struct grcan_priv *pDev = d;
	struct grcan_timing timing;
	int ret;

	FUNCDBG();

	/* cannot change speed during run mode */
	if (pDev->started == STATE_STARTED)
		return -1;

	/* get speed rate from argument */
	ret = grcan_calc_timing(speed, pDev->corefreq_hz, GRCAN_SAMPLING_POINT, &timing);
	if ( ret )
		return -2;

	/* save timing/speed */
	pDev->config.timing = timing;
	pDev->config_changed = 1;

	return 0;
}

int grcan_set_btrs(void *d, const struct grcan_timing *timing)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	/* Set BTR registers manually
	 * Read GRCAN/HurriCANe Manual.
	 */
	if (pDev->started == STATE_STARTED)
		return -1;

	if ( !timing )
		return -2;

	pDev->config.timing = *timing;
	pDev->config_changed = 1;

	return 0;
}

int grcan_set_afilter(void *d, const struct grcan_filter *filter)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if ( !filter ){
		/* Disable filtering - let all messages pass */
		pDev->afilter.mask = 0x0;
		pDev->afilter.code = 0x0;
	}else{
		/* Save filter */
		pDev->afilter = *filter;
	}
	/* Set hardware acceptance filter */
	grcan_hw_accept(pDev->regs,&pDev->afilter);

	return 0;
}

int grcan_set_sfilter(void *d, const struct grcan_filter *filter)
{
	struct grcan_priv *pDev = d;
	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	if ( !filter ){
		/* disable TX/RX SYNC filtering */
		pDev->sfilter.mask = 0xffffffff;
		pDev->sfilter.mask = 0;

		 /* disable Sync interrupt */
		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~(GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}else{
		/* Save filter */
		pDev->sfilter = *filter;

		/* Enable Sync interrupt */
		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		pDev->regs->imr = READ_REG(&pDev->regs->imr) | (GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}
	/* Set Sync RX/TX filter */
	grcan_hw_sync(pDev->regs,&pDev->sfilter);

	return 0;
}

int grcan_get_status(void* d, unsigned int *data)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	if ( !data )
		return -1;

	/* Read out the statsu register from the GRCAN core */
	data[0] = READ_REG(&pDev->regs->stat);

	return 0;
}

/* Error indicators */
#define GRCAN_IRQ_ERRORS \
		(GRCAN_RXAHBERR_IRQ | GRCAN_TXAHBERR_IRQ | GRCAN_OFF_IRQ)
#define GRCAN_STAT_ERRORS (GRCAN_STAT_AHBERR | GRCAN_STAT_OFF)
/* Warning & RX/TX sync indicators */
#define GRCAN_IRQ_WARNS \
		(GRCAN_ERR_IRQ | GRCAN_OR_IRQ | GRCAN_TXLOSS_IRQ | \
		 GRCAN_RXSYNC_IRQ | GRCAN_TXSYNC_IRQ)
#define GRCAN_STAT_WARNS (GRCAN_STAT_OR | GRCAN_STAT_PASS)

/* Handle the IRQ */
static void grcan_interrupt(void *arg)
{
	struct grcan_priv *pDev = arg;
	unsigned int status = READ_REG(&pDev->regs->pimsr);
	unsigned int canstat = READ_REG(&pDev->regs->stat);
	unsigned int imr_clear;
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Spurious IRQ call? */
	if ( !status && !canstat )
		return;

	if (pDev->started != STATE_STARTED) {
		DBGC(DBG_STATE, "not STARTED (unexpected interrupt)\n");
		pDev->regs->picr = status;
		return;
	}

	FUNCDBG();

	if ( (status & GRCAN_IRQ_ERRORS) || (canstat & GRCAN_STAT_ERRORS) ) {
		/* Bus-off condition interrupt
		 * The link is brought down by hardware, we wake all threads
		 * that is blocked in read/write calls and stop futher calls
		 * to read/write until user has called ioctl(fd,START,0).
		 */
		SPIN_LOCK(&pDev->devlock, irqflags);
		DBGC(DBG_STATE, "STARTED->BUSOFF|AHBERR\n");
		pDev->stats.ints++;
		if ((status & GRCAN_OFF_IRQ) || (canstat & GRCAN_STAT_OFF)) {
			/* CAN Bus-off interrupt */
			DBGC(DBG_STATE, "BUSOFF: status: 0x%x, canstat: 0x%x\n",
				status, canstat);
			pDev->started = STATE_BUSOFF;
			pDev->stats.busoff_cnt++;
		} else {
			/* RX or Tx AHB Error interrupt */
			printk("AHBERROR: status: 0x%x, canstat: 0x%x\n",
				status, canstat);
			pDev->started = STATE_AHBERR;
			pDev->stats.ahberr_cnt++;
		}
		grcan_hw_stop(pDev); /* this mask all IRQ sources */
		pDev->regs->picr = 0x1ffff; /* clear all interrupts */
		/*
		 * Prevent driver from affecting bus. Driver can be started
		 * again with grcan_start().
		 */
		SPIN_UNLOCK(&pDev->devlock, irqflags);

		/* Release semaphores to wake blocked threads. */
		grcan_sw_stop(pDev);

		/*
		 * NOTE: Another interrupt may be pending now so ISR could be
		 * executed one more time aftert this (first) return.
		 */
		return;
	}

	/* Mask interrupts in one place under spin-lock. */
	imr_clear = status & (GRCAN_RXIRQ_IRQ | GRCAN_TXIRQ_IRQ | GRCAN_TXEMPTY_IRQ);

	SPIN_LOCK(&pDev->devlock, irqflags);

	/* Increment number of interrupts counter */
	pDev->stats.ints++;
	if ((status & GRCAN_IRQ_WARNS) || (canstat & GRCAN_STAT_WARNS)) {

		if ( (status & GRCAN_ERR_IRQ) || (canstat & GRCAN_STAT_PASS) ) {
			/* Error-Passive interrupt */
			pDev->stats.passive_cnt++;
		}

		if ( (status & GRCAN_OR_IRQ) || (canstat & GRCAN_STAT_OR) ) {
			/* Over-run during reception interrupt */
			pDev->stats.overrun_cnt++;
		}

		if ( status & GRCAN_TXLOSS_IRQ ) {
			pDev->stats.txloss_cnt++;
		}

		if ( status & GRCAN_TXSYNC_IRQ ) {
			/* TxSync message transmitted interrupt */
			pDev->stats.txsync_cnt++;
		}

		if ( status & GRCAN_RXSYNC_IRQ ) {
			/* RxSync message received interrupt */
			pDev->stats.rxsync_cnt++;
		}
	}

	if (imr_clear) {
		pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~imr_clear;

		SPIN_UNLOCK(&pDev->devlock, irqflags);

		if ( status & GRCAN_RXIRQ_IRQ ) {
			/* RX IRQ pointer interrupt */
			rtems_semaphore_release(pDev->rx_sem);
		}

		if ( status & GRCAN_TXIRQ_IRQ ) {
			/* TX IRQ pointer interrupt */
			rtems_semaphore_release(pDev->tx_sem);
		}

		if (status & GRCAN_TXEMPTY_IRQ ) {
			rtems_semaphore_release(pDev->txempty_sem);
		}
	} else {
		SPIN_UNLOCK(&pDev->devlock, irqflags);
	}

	/* Clear IRQs */
	pDev->regs->picr = status;
}
