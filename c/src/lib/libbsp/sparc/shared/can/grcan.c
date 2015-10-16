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
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <bsp/grcan.h>
#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <ambapp.h>

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

#ifndef IRQ_GLOBAL_PREPARE
 #define IRQ_GLOBAL_PREPARE(level) rtems_interrupt_level level
#endif

#ifndef IRQ_GLOBAL_DISABLE
 #define IRQ_GLOBAL_DISABLE(level) rtems_interrupt_disable(level)
#endif

#ifndef IRQ_GLOBAL_ENABLE
 #define IRQ_GLOBAL_ENABLE(level) rtems_interrupt_enable(level)
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
#define DBG_IOCTRL 1
#define DBG_TX 2
#define DBG_RX 4

#define DEBUG_FLAGS (DBG_IOCTRL | DBG_RX | DBG_TX )
/*
#define DEBUG
#define DEBUGFUNCS
*/
#include <bsp/debug_defs.h>

/*********************************************************/

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
	int txerror, rxerror;

	struct grcan_filter sfilter;
	struct grcan_filter afilter;
	int config_changed; /* 0=no changes, 1=changes ==> a Core reset is needed */
	struct grcan_config config;
	struct grcan_stats stats;

	rtems_id rx_sem, tx_sem, txempty_sem, dev_sem;
};

static rtems_device_driver grcan_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg);
static rtems_device_driver grcan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grcan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grcan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grcan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grcan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define GRCAN_DRIVER_TABLE_ENTRY { grcan_initialize, grcan_open, grcan_close, grcan_read, grcan_write, grcan_ioctl }

static void __inline__ grcan_hw_reset(struct grcan_regs *regs);

static unsigned int grcan_hw_read_try(
  struct grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int max);

static unsigned int grcan_hw_write_try(
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
#define READ_REG(address) _grcan_read_nocache((unsigned int)(address))
#else
#define READ_REG(address) (*(volatile unsigned int *)(address))
#endif

#ifdef GRCAN_DMA_BYPASS_CACHE
#define READ_DMA_WORD(address) _grcan_read_nocache((unsigned int)(address))
#define READ_DMA_BYTE(address) _grcan_read_nocache_byte((unsigned int)(address))
static unsigned char __inline__ _grcan_read_nocache_byte(unsigned int address)
{
	unsigned char tmp;
	__asm__ (" lduba [%1]1, %0 "
	    : "=r"(tmp)
	    : "r"(address)
	);
	return tmp;
}
#else
#define READ_DMA_WORD(address) (*(volatile unsigned int *)(address))
#define READ_DMA_BYTE(address) (*(volatile unsigned char *)(address))
#endif

#if defined(GRCAN_REG_BYPASS_CACHE) || defined(GRCAN_DMA_BYPASS_CACHE)
static unsigned int __inline__ _grcan_read_nocache(unsigned int address)
{
	unsigned int tmp;
	__asm__ (" lda [%1]1, %0 "
	        : "=r"(tmp)
	        : "r"(address)
	);
	return tmp;
}
#endif


static rtems_driver_address_table grcan_driver = GRCAN_DRIVER_TABLE_ENTRY;
static int grcan_driver_io_registered = 0;
static rtems_device_major_number grcan_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int grcan_register_io(rtems_device_major_number *m);
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
	priv = dev->priv = malloc(sizeof(struct grcan_priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	memset(priv, 0, sizeof(*priv));
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int grcan_init3(struct drvmgr_dev *dev)
{
	struct grcan_priv *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( grcan_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( grcan_register_io(&grcan_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		grcan_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	if ( grcan_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/grcan%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgrcan%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, grcan_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

int grcan_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &grcan_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("GRCAN driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("GRCAN rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("GRCAN rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("GRCAN rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("GRCAN rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
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

static rtems_device_driver grcan_start(struct grcan_priv *pDev)
{
  /*
   * tmp is set but never used. GCC gives a warning for this
   * and we need to tell GCC not to complain.
   */
  unsigned int tmp RTEMS_UNUSED;

  IRQ_GLOBAL_PREPARE(oldLevel);

  FUNCDBG();

  /* Check that memory has been allocated successfully */
  if ( !pDev->tx || !pDev->rx )
    return RTEMS_NO_MEMORY;

  /* Configure FIFO configuration register
   * and Setup timing
   */
  if ( pDev->config_changed ){
    grcan_hw_config(pDev->regs,&pDev->config);
    pDev->config_changed = 0;
  }

  /* Setup receiver */
  pDev->regs->rx0addr = (unsigned int)pDev->_rx_hw;
  pDev->regs->rx0size = pDev->rxbuf_size;

  /* Setup Transmitter */
  pDev->regs->tx0addr = (unsigned int)pDev->_tx_hw;
  pDev->regs->tx0size = pDev->txbuf_size;

  /* Setup acceptance filters */
  grcan_hw_accept(pDev->regs,&pDev->afilter);

  /* Sync filters */
  grcan_hw_sync(pDev->regs,&pDev->sfilter);

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
  IRQ_GLOBAL_DISABLE(oldLevel);
  IRQ_UNMASK(pDev->irq+GRCAN_IRQ_TXSYNC);
  IRQ_UNMASK(pDev->irq+GRCAN_IRQ_RXSYNC);
  IRQ_UNMASK(pDev->irq+GRCAN_IRQ_IRQ);
  IRQ_GLOBAL_ENABLE(oldLevel);

  /* Reset some software data */
  /*pDev->txerror = 0;
  pDev->rxerror = 0;*/

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

static void grcan_stop(struct grcan_priv *pDev)
{
  FUNCDBG();

  /* Mask all IRQs */
  pDev->regs->imr = 0;
  IRQ_MASK(pDev->irq+GRCAN_IRQ_TXSYNC);
  IRQ_MASK(pDev->irq+GRCAN_IRQ_RXSYNC);
  IRQ_MASK(pDev->irq+GRCAN_IRQ_IRQ);

  /* Disable receiver & transmitter */
  pDev->regs->rx0ctrl = 0;
  pDev->regs->tx0ctrl = 0;

  /* Reset semaphores to the initial state and wakeing
   * all threads waiting for an IRQ. The threads that
   * get woken up must check for RTEMS_UNSATISFIED in
   * order to determine that they should return to
   * user space with error status.
   */
  rtems_semaphore_flush(pDev->rx_sem);
  rtems_semaphore_flush(pDev->tx_sem);
  rtems_semaphore_flush(pDev->txempty_sem);
}

static void grcan_hw_config(
  struct grcan_regs *regs,
  struct grcan_config *conf
  )
{
  unsigned int config=0;

  /* Reset HurriCANe Core */
  regs->ctrl = 0;

  if ( conf->silent )
    config |= GRCAN_CFG_SILENT;

  if ( conf->abort )
    config |= GRCAN_CFG_ABORT;

  if ( conf->selection.selection )
    config |= GRCAN_CFG_SELECTION;

  if ( conf->selection.enable0 )
    config |= GRCAN_CFG_ENABLE0;

  if ( conf->selection.enable1 )
    config |= GRCAN_CFG_ENABLE1;

  /* Timing */
  config |= (conf->timing.bpr<<GRCAN_CFG_BPR_BIT) & GRCAN_CFG_BPR;
  config |= (conf->timing.rsj<<GRCAN_CFG_RSJ_BIT) & GRCAN_CFG_RSJ;
  config |= (conf->timing.ps1<<GRCAN_CFG_PS1_BIT) & GRCAN_CFG_PS1;
  config |= (conf->timing.ps2<<GRCAN_CFG_PS2_BIT) & GRCAN_CFG_PS2;
  config |= (conf->timing.scaler<<GRCAN_CFG_SCALER_BIT) & GRCAN_CFG_SCALER;

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

static void grcan_hw_sync(
  struct grcan_regs *regs,
  struct grcan_filter *sfilter
  )
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
  unsigned int wp,
  unsigned int size
  )
{
  if ( rp == wp ) {
    /* read pointer and write pointer is equal only
     * when RX buffer is empty.
     */
    return 0;
  }

  if ( wp > rp ) {
    return (wp-rp)/GRCAN_MSG_SIZE;
  }else{
    return (size-(rp-wp))/GRCAN_MSG_SIZE;
  }
}

static unsigned int grcan_hw_txspace(
  unsigned int rp,
  unsigned int wp,
  unsigned int size
  )
{
  unsigned int left;

  if ( rp == wp ) {
    /* read pointer and write pointer is equal only
     * when TX buffer is empty.
     */
    return size/GRCAN_MSG_SIZE-WRAP_AROUND_TX_MSGS;
  }

  /* size - 4 - abs(read-write) */
  if ( wp > rp ) {
    left = size-(wp-rp);
  }else{
    left = rp-wp;
  }

  return left/GRCAN_MSG_SIZE-WRAP_AROUND_TX_MSGS;
}

static int grcan_hw_rx_ongoing(struct grcan_regs *regs)
{
  return READ_REG(&regs->rx0ctrl) & GRCAN_RXCTRL_ONGOING;
};

static int grcan_hw_tx_ongoing(struct grcan_regs *regs)
{
  return READ_REG(&regs->tx0ctrl) & GRCAN_TXCTRL_ONGOING;
};


#define MIN_TSEG1 1
#define MIN_TSEG2 2
#define MAX_TSEG1 14
#define MAX_TSEG2 8

static int grcan_calc_timing(
  unsigned int baud,          /* The requested BAUD to calculate timing for */
  unsigned int core_hz,       /* Frequency in Hz of GRCAN Core */
  unsigned int sampl_pt,
  struct grcan_timing *timing /* result is placed here */
  )
{
	int best_error = 1000000000;
	int error;
	int best_tseg=0, best_brp=0, brp=0;
	int tseg=0, tseg1=0, tseg2=0;
	int sjw = 1;

  /* Default to 90% */
  if ( (sampl_pt < 50) || (sampl_pt>99) ){
    sampl_pt = GRCAN_SAMPLING_POINT;
  }

	if ( (baud<5000) || (baud>1000000) ){
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
	for (tseg = (MIN_TSEG1 + MIN_TSEG2 + 2) * 2;
	     tseg <= (MAX_TSEG2 + MAX_TSEG1 + 2) * 2 + 1;
	     tseg++)
	{
		brp = core_hz / ((1 + tseg / 2) * baud) + tseg % 2;
		if ((brp <= 0) ||
        ( (brp > 256*1) && (brp <= 256*2) && (brp&0x1) ) ||
        ( (brp > 256*2) && (brp <= 256*4) && (brp&0x3) ) ||
        ( (brp > 256*4) && (brp <= 256*8) && (brp&0x7) ) ||
        (brp > 256*8)
        )
			continue;

		error = baud - core_hz / (brp * (1 + tseg / 2));
		if (error < 0)
		{
			error = -error;
		}

		if (error <= best_error)
		{
			best_error = error;
			best_tseg = tseg/2;
			best_brp = brp-1;
		}
	}

	if (best_error && (baud / best_error < 10))
	{
		return -2;
	}else if ( !timing )
		return 0; /* nothing to store result in, but a valid bitrate can be calculated */

	tseg2 = best_tseg - (sampl_pt * (best_tseg + 1)) / 100;

	if (tseg2 < MIN_TSEG2)
	{
		tseg2 = MIN_TSEG2;
	}

	if (tseg2 > MAX_TSEG2)
	{
		tseg2 = MAX_TSEG2;
	}

	tseg1 = best_tseg - tseg2 - 2;

	if (tseg1 > MAX_TSEG1)
	{
		tseg1 = MAX_TSEG1;
		tseg2 = best_tseg - tseg1 - 2;
	}

  /* Get scaler and BRP from pseudo BRP */
  if ( best_brp <= 256 ){
    timing->scaler = best_brp;
    timing->bpr = 0;
  }else if ( best_brp <= 256*2 ){
    timing->scaler = ((best_brp+1)>>1) -1;
    timing->bpr = 1;
  }else if ( best_brp <= 256*4 ){
    timing->scaler = ((best_brp+1)>>2) -1;
    timing->bpr = 2;
  }else{
    timing->scaler = ((best_brp+1)>>3) -1;
    timing->bpr = 3;
  }

	timing->ps1    = tseg1+1;
	timing->ps2    = tseg2;
	timing->rsj    = sjw;

	return 0;
}

static unsigned int grcan_hw_read_try(
  struct grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int max
  )
{
  int i,j;
  CANMsg *dest;
  struct grcan_msg *source,tmp;
  unsigned int wp,rp,size,rxmax,addr,trunk_msg_cnt;

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
  if ( wp != rp ){
    /* Not empty, we have received chars...
     * Read as much as possible from DMA buffer
     */
    size = READ_REG(&regs->rx0size);

    /* Get number of bytes available in RX buffer */
    trunk_msg_cnt = grcan_hw_rxavail(rp,wp,size);

    /* truncate size if user space buffer hasn't room for
     * all received chars.
     */
    if ( trunk_msg_cnt > max )
      trunk_msg_cnt = max;

    /* Read until i is 0 */
    i=trunk_msg_cnt;

    addr = (unsigned int)pDev->rx;
    source = (struct grcan_msg *)(addr + rp);
    dest = buffer;
    rxmax = addr + (size-GRCAN_MSG_SIZE);

    /* Read as many can messages as possible */
    while(i>0){
      /* Read CAN message from DMA buffer */
      tmp.head[0] = READ_DMA_WORD(&source->head[0]);
      tmp.head[1] = READ_DMA_WORD(&source->head[1]);
      /* Convert one grcan CAN message to one "software" CAN message */
      dest->extended = tmp.head[0]>>31;
      dest->rtr = (tmp.head[0] >>30) & 0x1;
      if ( dest->extended ){
        dest->id = tmp.head[0] & 0x3fffffff;
      }else{
        dest->id = (tmp.head[0] >>18) & 0xfff;
      }
      dest->len = tmp.head[1] >> 28;
      for(j=0; j<dest->len; j++)
        dest->data[j] = READ_DMA_BYTE(&source->data[j]);

      /* wrap around if neccessary */
      source = ( (unsigned int)source >= rxmax ) ? (struct grcan_msg *)addr : source+1;
      dest++; /* straight user buffer */
      i--;
    }
    /* Increment Hardware READ pointer (mark read byte as read)
     * ! wait for registers to be safely re-configurable
     */
    regs->rx0ctrl = 0; /* DISABLE RX CHANNEL */
    i=0;
    while( grcan_hw_rx_ongoing(regs) && (i<1000) ){
      i++;
    }
    regs->rx0rd = (unsigned int)source-addr;
    regs->rx0ctrl = GRCAN_RXCTRL_ENABLE; /* ENABLE_RX_CHANNEL */
    return trunk_msg_cnt;
  }
  return 0;
}

static unsigned int grcan_hw_write_try(
  struct grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int count
  )
{
  unsigned int rp, wp, size, txmax, addr, ret;
  struct grcan_msg *dest;
  CANMsg *source;
  int space_left;
  unsigned int tmp;
  int i;

  DBGC(DBG_TX,"\n");
  /*FUNCDBG();*/

  rp = READ_REG(&regs->tx0rd);
  wp = READ_REG(&regs->tx0wr);
  size = READ_REG(&regs->tx0size);

  space_left = grcan_hw_txspace(rp,wp,size);

  /* is circular fifo full? */
  if ( space_left < 1 )
    return 0;

  /* Truncate size */
  if ( space_left > count )
    space_left = count;
  ret = space_left;

  addr = (unsigned int)pDev->tx;

  dest = (struct grcan_msg *)(addr + wp);
  source = (CANMsg *)buffer;
  txmax = addr + (size-GRCAN_MSG_SIZE);

  while ( space_left>0 ) {
    /* Convert and write CAN message to DMA buffer */
    if ( source->extended ){
      tmp = (1<<31) | (source->id & 0x3fffffff);
    }else{
      tmp = (source->id&0xfff)<<18;
    }
    if ( source->rtr )
      tmp|=(1<<30);
    dest->head[0] = tmp;
    dest->head[1] = source->len<<28;
    for ( i=0; i<source->len; i++)
      dest->data[i] = source->data[i];
    source++; /* straight user buffer */
    dest = ((unsigned int)dest >= txmax) ? (struct grcan_msg *)addr : dest+1;
    space_left--;
  }

  /* Update write pointer
   * ! wait for registers to be safely re-configurable
   */
  regs->tx0ctrl = 0; /* DISABLE TX CHANNEL */
  i=0;
  while( (grcan_hw_tx_ongoing(regs)) && i<1000 ){
    i++;
  }
  regs->tx0wr = (unsigned int)dest - addr; /* Update write pointer */
  regs->tx0ctrl = GRCAN_TXCTRL_ENABLE; /* ENABLE_TX_CHANNEL */
  return ret;
}

static int grcan_wait_rxdata(
  struct grcan_priv *pDev,
  int min
  )
{
  unsigned int wp, rp, size, irq;
  unsigned int irq_trunk, dataavail;
  int wait;
  IRQ_GLOBAL_PREPARE(oldLevel);

  FUNCDBG();

  /*** block until receive IRQ received
   * Set up a valid IRQ point so that an IRQ is received
   * when one or more messages are received
   */
  IRQ_GLOBAL_DISABLE(oldLevel);

  size = READ_REG(&pDev->regs->rx0size);
  rp = READ_REG(&pDev->regs->rx0rd);
  wp = READ_REG(&pDev->regs->rx0wr);

  /**** Calculate IRQ Pointer ****/
  irq = wp + min*GRCAN_MSG_SIZE;
  /* wrap irq around */
  if ( irq >= size ){
    irq_trunk = irq-size;
  }else
    irq_trunk = irq;

  /* init IRQ HW */
  pDev->regs->rx0irq = irq_trunk;

  /* Clear pending Rx IRQ */
  pDev->regs->picr = GRCAN_RXIRQ_IRQ;

  wp = READ_REG(&pDev->regs->rx0wr);

  /* Calculate messages available */
  dataavail = grcan_hw_rxavail(rp,wp,size);

  if ( dataavail < min ){
    /* Still empty, proceed with sleep - Turn on IRQ (unmask irq) */
    pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_RXIRQ_IRQ;
    wait=1;
  }else{
    /* enough message has been received, abort sleep - don't unmask interrupt */
    wait=0;
  }
  IRQ_GLOBAL_ENABLE(oldLevel);

    /* Wait for IRQ to fire only if has been triggered */
  if ( wait ){
    if ( rtems_semaphore_obtain(pDev->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT) == RTEMS_UNSATISFIED )
      return -1; /* Device driver has been closed or stopped, return with error status */
  }

  return 0;
}

/* Wait until min bytes available in TX circular buffer.
 * The IRQ RxIrq is used to pin point the location of
 *
 * min must be at least WRAP_AROUND_TX_BYTES bytes less
 * than max buffer for this algo to work.
 *
 */
static int grcan_wait_txspace(
  struct grcan_priv *pDev,
  int min
  )
{
  int wait;
  unsigned int irq, rp, wp, size, space_left;
  unsigned int irq_trunk;
  IRQ_GLOBAL_PREPARE(oldLevel);

  DBGC(DBG_TX,"\n");
  /*FUNCDBG();*/

  IRQ_GLOBAL_DISABLE(oldLevel);

  pDev->regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;

  size = READ_REG(&pDev->regs->tx0size);
  wp = READ_REG(&pDev->regs->tx0wr);

  rp = READ_REG(&pDev->regs->tx0rd);

  /**** Calculate IRQ Pointer ****/
  irq = rp + min*GRCAN_MSG_SIZE;
  /* wrap irq around */
  if ( irq >= size ){
    irq_trunk = irq - size;
  }else
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

  space_left = grcan_hw_txspace(rp,wp,size);

  if ( space_left < min ){
    /* Still too full, proceed with sleep - Turn on IRQ (unmask irq) */
    pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_TXIRQ_IRQ;
    wait=1;
  }else{
    /* There are enough room in buffer, abort wait - don't unmask interrupt */
    wait=0;
  }
  IRQ_GLOBAL_ENABLE(oldLevel);

  /* Wait for IRQ to fire only if it has been triggered */
  if ( wait ){
    if ( rtems_semaphore_obtain(pDev->tx_sem, RTEMS_WAIT, 100) ==
         RTEMS_UNSATISFIED ){
      /* Device driver has flushed us, this may be due to another thread has
       * closed the device, this is to avoid deadlock */
      return -1;
    }
  }

  /* At this point the TxIRQ has been masked, we ned not to mask it */
  return 0;
}

static int grcan_tx_flush(struct grcan_priv *pDev)
{
  int wait;
  unsigned int rp, wp;
  IRQ_GLOBAL_PREPARE(oldLevel);
  FUNCDBG();

  /* loop until all data in circular buffer has been read by hw.
   * (write pointer != read pointer )
   *
   * Hardware doesn't update write pointer - we do
   */
  while ( (wp=READ_REG(&pDev->regs->tx0wr)) != (rp=READ_REG(&pDev->regs->tx0rd)) ) {
    /* Wait for TX empty IRQ */
    IRQ_GLOBAL_DISABLE(oldLevel);
    /* Clear pending TXEmpty IRQ */
    pDev->regs->picr = GRCAN_TXEMPTY_IRQ;

    if ( wp != READ_REG(&pDev->regs->tx0rd) ) {
      /* Still not empty, proceed with sleep - Turn on IRQ (unmask irq) */
      pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_TXEMPTY_IRQ;
      wait = 1;
    }else{
      /* TX fifo is empty */
      wait = 0;
    }
    IRQ_GLOBAL_ENABLE(oldLevel);
    if ( !wait )
      break;

    /* Wait for IRQ to wake us */
    if ( rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT) ==
         RTEMS_UNSATISFIED ) {
      return -1;
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
				pDev->_tx = malloc(pDev->txbuf_size +
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
				pDev->_rx = malloc(pDev->rxbuf_size +
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

  if ( tx && pDev->_tx ){
    free(pDev->_tx);
    pDev->_tx = NULL;
    pDev->tx = NULL;
  }

  if ( rx && pDev->_rx ){
    free(pDev->_rx);
    pDev->_rx = NULL;
    pDev->rx = NULL;
  }
}

static rtems_device_driver grcan_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number unused,
  void *arg
  )
{
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grcan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grcan_priv *pDev;
	rtems_device_driver ret;
	struct drvmgr_dev *dev;
	union drvmgr_key_value *value;

	FUNCDBG();

	if ( drvmgr_get_dev(&grcan_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	pDev = (struct grcan_priv *)dev->priv;

	/* Wait until we get semaphore */
	if (rtems_semaphore_obtain(pDev->dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL) {
		return RTEMS_INTERNAL_ERROR;
	}

	/* is device busy/taken? */
	if ( pDev->open ) {
		ret=RTEMS_RESOURCE_IN_USE;
		goto out;
	}

	/* Mark device taken */
	pDev->open = 1;

	pDev->txblock = pDev->rxblock = 1;
	pDev->txcomplete = pDev->rxcomplete = 0;
	pDev->started = 0;
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
		ret=RTEMS_NO_MEMORY;
		goto out;
	}

	/* Clear statistics */
	memset(&pDev->stats,0,sizeof(struct grcan_stats));

	ret = RTEMS_SUCCESSFUL;
out:
	rtems_semaphore_release(pDev->dev_sem);
	return ret;
}

static rtems_device_driver grcan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grcan_priv *pDev;
	struct drvmgr_dev *dev;

	FUNCDBG();

	if ( drvmgr_get_dev(&grcan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (struct grcan_priv *)dev->priv;

	if ( pDev->started )
		grcan_stop(pDev);

	grcan_hw_reset(pDev->regs);

	grcan_free_buffers(pDev,1,1);

	/* Mark Device as closed */
	pDev->open = 0;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grcan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grcan_priv *pDev;
	struct drvmgr_dev *dev;
	rtems_libio_rw_args_t *rw_args;  
	CANMsg *dest;
	unsigned int count, left;
	int req_cnt;

	FUNCDBG();

	if ( drvmgr_get_dev(&grcan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (struct grcan_priv *)dev->priv;

	rw_args = (rtems_libio_rw_args_t *) arg;
	dest = (CANMsg *) rw_args->buffer;
	req_cnt = rw_args->count / sizeof(CANMsg);

	if ( (!dest) || (req_cnt<1) )
		return RTEMS_INVALID_NAME;

	if ( !pDev->started )
		return RTEMS_RESOURCE_IN_USE;

	/*FUNCDBG("grcan_read [%i,%i]: buf: 0x%x len: %i\n",major, minor, (unsigned int)rw_args->buffer,rw_args->count);*/

	count = grcan_hw_read_try(pDev,pDev->regs,dest,req_cnt);
	if ( !( pDev->rxblock && pDev->rxcomplete && (count!=req_cnt) ) ){
		if ( count > 0 ) {
			/* Successfully received messages (at least one) */
			rw_args->bytes_moved = count * sizeof(CANMsg);
			return RTEMS_SUCCESSFUL;
		}

		/* nothing read, shall we block? */
		if ( !pDev->rxblock ) {
			/* non-blocking mode */
			rw_args->bytes_moved = 0;
			return RTEMS_TIMEOUT;
		}
	}

	while(count == 0 || (pDev->rxcomplete && (count!=req_cnt)) ){

	if ( !pDev->rxcomplete ){
		left = 1; /* return as soon as there is one message available */
	}else{
		left = req_cnt - count;     /* return as soon as all data are available */

		/* never wait for more than the half the maximum size of the receive buffer 
		 * Why? We need some time to copy buffer before to catch up with hw,
		 * otherwise we would have to copy everything when the data has been
		 * received.
		 */
		if ( left > ((pDev->rxbuf_size/GRCAN_MSG_SIZE)/2) ){
			left = (pDev->rxbuf_size/GRCAN_MSG_SIZE)/2;
		}
	}

	if ( grcan_wait_rxdata(pDev,left) ) {
		/* The wait has been aborted, probably due to 
		 * the device driver has been closed by another
		 * thread.
		 */
		rw_args->bytes_moved = count * sizeof(CANMsg);
		return RTEMS_UNSATISFIED;
	}

	/* Try read bytes from circular buffer */
	count += grcan_hw_read_try(
			pDev,
			pDev->regs,
			dest+count,
			req_cnt-count);
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */
	rw_args->bytes_moved = count * sizeof(CANMsg);
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grcan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grcan_priv *pDev;
	struct drvmgr_dev *dev;
	rtems_libio_rw_args_t *rw_args;
	CANMsg *source;
	unsigned int count, left;
	int req_cnt;

	DBGC(DBG_TX,"\n");

	if ( drvmgr_get_dev(&grcan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (struct grcan_priv *)dev->priv;

	if ( !pDev->started || pDev->config.silent || pDev->flushing )
		return RTEMS_RESOURCE_IN_USE;

	rw_args = (rtems_libio_rw_args_t *) arg;
	req_cnt = rw_args->count / sizeof(CANMsg);
	source = (CANMsg *) rw_args->buffer;

	/* check proper length and buffer pointer */
	if (( req_cnt < 1) || (source == NULL) ){
		return RTEMS_INVALID_NAME;
	}

	count = grcan_hw_write_try(pDev,pDev->regs,source,req_cnt);
	if ( !(pDev->txblock && pDev->txcomplete && (count!=req_cnt)) ) {
		if ( count > 0 ) {
			/* Successfully transmitted chars (at least one char) */
			rw_args->bytes_moved = count * sizeof(CANMsg);
			return RTEMS_SUCCESSFUL;
		}

		/* nothing written, shall we block? */
		if ( !pDev->txblock ) {
			/* non-blocking mode */
			rw_args->bytes_moved = 0;
			return RTEMS_TIMEOUT;
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

		/* Wait until more room in transmit buffer */
		if ( grcan_wait_txspace(pDev,left) ){
			/* The wait has been aborted, probably due to 
			 * the device driver has been closed by another
			 * thread. To avoid deadlock we return directly
			 * with error status.
			 */
			rw_args->bytes_moved = count * sizeof(CANMsg);
			return RTEMS_UNSATISFIED;
		}

		if ( pDev->txerror ){
			/* Return number of bytes sent, compare write pointers */
			pDev->txerror = 0;
#if 0 
#error HANDLE AMBA error
#endif
		}

		/* Try read bytes from circular buffer */
		count += grcan_hw_write_try(
			pDev,
			pDev->regs,
			source+count,
			req_cnt-count);
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */

	rw_args->bytes_moved = count * sizeof(CANMsg);
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grcan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grcan_priv *pDev;
	struct drvmgr_dev *dev;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *)arg;
	unsigned int *data = ioarg->buffer;
	struct grcan_timing timing;
	unsigned int speed;
	struct grcan_selection *selection;
	int tmp,ret;
	rtems_device_driver status;
	struct grcan_stats *stats;
	struct grcan_filter *filter;
	IRQ_GLOBAL_PREPARE(oldLevel);

	FUNCDBG();

	if ( drvmgr_get_dev(&grcan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	pDev = (struct grcan_priv *)dev->priv;

	if (!ioarg)
		return RTEMS_INVALID_NAME;

	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
		case GRCAN_IOC_START:
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */

		if ( (status=grcan_start(pDev)) != RTEMS_SUCCESSFUL ){
			return status;
		}
		/* Read and write are now open... */
		pDev->started = 1;

		/* Register interrupt routine and enable IRQ at IRQ ctrl */
		drvmgr_interrupt_register(dev, 0, "grcan", grcan_interrupt, pDev);

		break;

		case GRCAN_IOC_STOP:
		if ( !pDev->started )
			return RTEMS_RESOURCE_IN_USE;

		/* Disable interrupts */
		drvmgr_interrupt_unregister(dev, 0, grcan_interrupt, pDev);

		grcan_stop(pDev);
		pDev->started = 0;
		break;

		case GRCAN_IOC_ISSTARTED:
		if ( !pDev->started )
			return RTEMS_RESOURCE_IN_USE;
		break;

		case GRCAN_IOC_FLUSH:
		if ( !pDev->started || pDev->flushing || pDev->config.silent )
			return RTEMS_RESOURCE_IN_USE;

		pDev->flushing = 1;
		tmp = grcan_tx_flush(pDev);
		pDev->flushing = 0;
		if ( tmp ) {
			/* The wait has been aborted, probably due to 
			 * the device driver has been closed by another
			 * thread.
			 */
			return RTEMS_UNSATISFIED;
		}
		break;

#if 0
		/* Set physical link */
		case GRCAN_IOC_SET_LINK:
#ifdef REDUNDANT_CHANNELS
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */

		/* switch HW channel */
		pDev->channel = (unsigned int)ioargs->buffer;
#else
		return RTEMS_NOT_IMPLEMENTED;
#endif
		break;
#endif

		case GRCAN_IOC_SET_SILENT:
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE;
		pDev->config.silent = (int)ioarg->buffer;
		pDev->config_changed = 1;
		break;

		case GRCAN_IOC_SET_ABORT:
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE;
		pDev->config.abort = (int)ioarg->buffer;
		/* This Configuration parameter doesn't need HurriCANe reset
		 * ==> no pDev->config_changed = 1;
		 */
		break;

		case GRCAN_IOC_SET_SELECTION:
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE;

		selection = (struct grcan_selection *)ioarg->buffer;
		if ( !selection )
			return RTEMS_INVALID_NAME;

		pDev->config.selection = *selection;
		pDev->config_changed = 1;
		break;

		case GRCAN_IOC_SET_RXBLOCK:
		pDev->rxblock = (int)ioarg->buffer;
		break;

		case GRCAN_IOC_SET_TXBLOCK:
		pDev->txblock = (int)ioarg->buffer;
		break;

		case GRCAN_IOC_SET_TXCOMPLETE:
		pDev->txcomplete = (int)ioarg->buffer;
		break;

		case GRCAN_IOC_SET_RXCOMPLETE:
		pDev->rxcomplete = (int)ioarg->buffer;
		break;

		case GRCAN_IOC_GET_STATS:
		stats = (struct grcan_stats *)ioarg->buffer;
		if ( !stats )
			return RTEMS_INVALID_NAME;
		*stats = pDev->stats;
		break;

		case GRCAN_IOC_CLR_STATS:
		IRQ_GLOBAL_DISABLE(oldLevel);
		memset(&pDev->stats,0,sizeof(struct grcan_stats));
		IRQ_GLOBAL_ENABLE(oldLevel);
		break;

		case GRCAN_IOC_SET_SPEED:
		/* cannot change speed during run mode */
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */

		/* get speed rate from argument */
		speed = (unsigned int)ioarg->buffer;
		ret = grcan_calc_timing(speed, pDev->corefreq_hz, GRCAN_SAMPLING_POINT, &timing);
		if ( ret )
			return  RTEMS_INVALID_NAME; /* EINVAL */

		/* save timing/speed */
		pDev->config.timing = timing;
		pDev->config_changed = 1;
		break;

		case GRCAN_IOC_SET_BTRS:
		/* Set BTR registers manually 
		 * Read GRCAN/HurriCANe Manual.
		 */
		if ( pDev->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */
		if ( !ioarg->buffer )
			return RTEMS_INVALID_NAME;

		pDev->config.timing = *(struct grcan_timing *)ioarg->buffer;
		pDev->config_changed = 1;
		break;

		case GRCAN_IOC_SET_AFILTER:
		filter = (struct grcan_filter *)ioarg->buffer;
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
		break;

		case GRCAN_IOC_SET_SFILTER:
		filter = (struct grcan_filter *)ioarg->buffer;
		if ( !filter ){
			/* disable TX/RX SYNC filtering */
			pDev->sfilter.mask = 0xffffffff; 
			pDev->sfilter.mask = 0;

			 /* disable Sync interrupt */
			pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~(GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
		}else{
			/* Save filter */
			pDev->sfilter = *filter;

			/* Enable Sync interrupt */
			pDev->regs->imr = READ_REG(&pDev->regs->imr) | (GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
		}
		/* Set Sync RX/TX filter */
		grcan_hw_sync(pDev->regs,&pDev->sfilter);
		break;

		case GRCAN_IOC_GET_STATUS:
		if ( !data )
			return RTEMS_INVALID_NAME;
		/* Read out the statsu register from the GRCAN core */
		data[0] = READ_REG(&pDev->regs->stat);
		break;

	default:
		return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}

/* Handle the IRQ */
static void grcan_interrupt(void *arg)
{
	struct grcan_priv *pDev = arg;
	unsigned int status = READ_REG(&pDev->regs->pimsr);
	unsigned int canstat = READ_REG(&pDev->regs->stat);

	/* Spurious IRQ call? */
	if ( !status && !canstat )
		return;

	FUNCDBG();

	/* Increment number of interrupts counter */
	pDev->stats.ints++;

	if ( (status & GRCAN_ERR_IRQ) || (canstat & GRCAN_STAT_PASS) ){
		/* Error-Passive interrupt */
		pDev->stats.passive_cnt++;
	}

	if ( (status & GRCAN_OFF_IRQ) || (canstat & GRCAN_STAT_OFF) ){
		/* Bus-off condition interrupt 
		 * The link is brought down by hardware, we wake all threads
		 * that is blocked in read/write calls and stop futher calls
		 * to read/write until user has called ioctl(fd,START,0).
		 */
		pDev->started = 0;
		grcan_stop(pDev); /* this mask all IRQ sources */
		status=0x1ffff; /* clear all interrupts */
		goto out;
	}

	if ( (status & GRCAN_OR_IRQ) || (canstat & GRCAN_STAT_OR) ){
		/* Over-run during reception interrupt */
		pDev->stats.overrun_cnt++;
	}

	if ( (status & GRCAN_RXAHBERR_IRQ) ||
	     (status & GRCAN_TXAHBERR_IRQ) ||
	     (canstat & GRCAN_STAT_AHBERR) ){
		/* RX or Tx AHB Error interrupt */
		printk("AHBERROR: status: 0x%x, canstat: 0x%x\n",status,canstat);
		pDev->stats.ahberr_cnt++;
	}

	if ( status & GRCAN_TXLOSS_IRQ ) {
		pDev->stats.txloss_cnt++;
	}

	if ( status & GRCAN_RXIRQ_IRQ ){
		/* RX IRQ pointer interrupt */
		/*printk("RxIrq 0x%x\n",status);*/
		pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~GRCAN_RXIRQ_IRQ;
		rtems_semaphore_release(pDev->rx_sem);
	}

	if ( status & GRCAN_TXIRQ_IRQ ){
		/* TX IRQ pointer interrupt */
		pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~GRCAN_TXIRQ_IRQ;
		rtems_semaphore_release(pDev->tx_sem);
	}

	if ( status & GRCAN_TXSYNC_IRQ ){
		/* TxSync message transmitted interrupt */
		pDev->stats.txsync_cnt++;
	}

	if ( status & GRCAN_RXSYNC_IRQ ){
		/* RxSync message received interrupt */
		pDev->stats.rxsync_cnt++;
	}

	if ( status & GRCAN_TXEMPTY_IRQ ){
		pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~GRCAN_TXEMPTY_IRQ;
		rtems_semaphore_release(pDev->txempty_sem);
	}

out:
	/* Clear IRQs */
	pDev->regs->picr = status;
}
