/*
 *  SatCAN FPGA driver
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bsp.h>
#include <rtems/bspIo.h> /* printk */

#include <grlib/satcan.h>
#include <grlib/ambapp.h>

#include <grlib/grlib_impl.h>

#ifndef GAISLER_SATCAN
#define GAISLER_SATCAN 0x080
#endif

#if !defined(SATCAN_DEVNAME) 
 #undef SATCAN_DEVNAME
 #define SATCAN_DEVNAME "/dev/satcan"
#endif

/* Enable debug output? */
/* #define DEBUG */ 

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif


/* Defines related to DMA */
#define ALIGN_2KMEM 32*1024
#define ALIGN_8KMEM 128*1024

#define OFFSET_2K_LOW_POS 15
#define OFFSET_8K_LOW_POS 17

#define DMA_2K_DATA_SELECT (1 << 14)
#define DMA_8K_DATA_SELECT (1 << 16)

#define DMA_2K_DATA_OFFSET 16*1024
#define DMA_8K_DATA_OFFSET 64*1024

/* Core register structures and defines */

/* Indexes to SatCAN registers in satcan array are declared in satcan.h*/
/* Fields for some of the SatCAN FPGA registers */

/* CmdReg0 */
#define CAN_TODn_Int_sel   (1 << 5)

/* CmdReg1 */
#define Sel_2k_8kN         (1 << 0)

/* Read FIFO */
#define FIFO_Full          (1 << 8)
#define FIFO_Empty         (1 << 9)

/* DMA Ch_Enable */
#define DMA_AutoInitDmaTx  (1 << 3)
#define DMA_EnTx2          (1 << 2)
#define DMA_EnTx1          (1 << 1)
#define DMA_EnRx           (1 << 0)

/* SatCAN wrapper register fields */
#define CTRL_BT_P     9
#define CTRL_NODENO_P 5
#define CTRL_DIS      (1 << 2)
#define CTRL_DPS_P    1
#define CTRL_RST      (1 << 0)

#define IRQ_AHB       (1 << 8)
#define IRQ_PPS       (1 << 7)
#define IRQ_M5        (1 << 6)
#define IRQ_M4        (1 << 5)
#define IRQ_M3        (1 << 4)
#define IRQ_M2        (1 << 3)
#define IRQ_M1        (1 << 2)
#define IRQ_SYNC      (1 << 1)
#define IRQ_CAN       (1 << 0)

#define MSK_AHB       (1 << 8)
#define MSK_PPS       (1 << 7)
#define MSK_M5        (1 << 6)
#define MSK_M4        (1 << 5)
#define MSK_M3        (1 << 4)
#define MSK_M2        (1 << 3)
#define MSK_M1        (1 << 2)
#define MSK_SYNC      (1 << 1)
#define MSK_CAN       (1 << 0)



struct satcan_regs {
	volatile unsigned int satcan[32];
	volatile unsigned int ctrl;
	volatile unsigned int irqpend;
	volatile unsigned int irqmask;
	volatile unsigned int membase;
};


struct satcan_priv {
	/* config */
	void           *dmaptr;
	unsigned char  *alptr;
	satcan_config  *cfg;
	
	/* driver state */
	rtems_id       devsem;
	rtems_id       txsem;
	int            open;
	int            txactive;
	int            dmaen;
	int            doff;
	rtems_interval timeout;
	int            dmamode;
};

static struct satcan_regs *regs;
static struct satcan_priv *priv;

static rtems_device_driver satcan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver satcan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver satcan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver satcan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver satcan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver satcan_initialize(rtems_device_major_number major, rtems_device_minor_number unused, void *arg);


/*
 * almalloc: allocate memory area of size sz aligned on sz boundary 
 * alptr: Utilized to return aligned pointer
 * ptr:   Unaligned pointer
 * sz:    Size of memory area
 */
static void almalloc(unsigned char **alptr, void **ptr, int sz)
{
  *ptr = rtems_calloc(1,2*sz);
  *alptr = (unsigned char *) (((int)*ptr+sz) & ~(sz-1));
}

static rtems_isr satcan_interrupt_handler(void *v)
{
	unsigned int irq;
	unsigned int fifo;
	
	irq = regs->irqpend;
	
	if (irq & IRQ_AHB && priv->cfg->ahb_irq_callback) {
		priv->cfg->ahb_irq_callback();
	}
	if (irq & IRQ_PPS && priv->cfg->pps_irq_callback) {
		priv->cfg->pps_irq_callback();
	}
	if (irq & IRQ_M5 && priv->cfg->m5_irq_callback) {
		priv->cfg->m5_irq_callback();
	}
	if (irq & IRQ_M4 && priv->cfg->m4_irq_callback) {
		priv->cfg->m4_irq_callback();
	}
	if (irq & IRQ_M3 && priv->cfg->m3_irq_callback) {
		priv->cfg->m3_irq_callback();
	}
	if (irq & IRQ_M2 && priv->cfg->m2_irq_callback) {
		priv->cfg->m2_irq_callback();
	}
	if (irq & IRQ_M1 && priv->cfg->m1_irq_callback) {
		priv->cfg->m1_irq_callback();
	}
	if (irq & IRQ_SYNC && priv->cfg->sync_irq_callback) {
		priv->cfg->sync_irq_callback();
	}
	if (irq & IRQ_CAN) {
		fifo = regs->satcan[SATCAN_FIFO];
		if (!(fifo & FIFO_Empty) && priv->txactive &&
		    (((fifo & 0xff) == SATCAN_IRQ_EOD1) || ((fifo & 0xff) == SATCAN_IRQ_EOD2))) { 
			rtems_semaphore_release(priv->txsem);
		}
		if (priv->cfg->can_irq_callback)
			priv->cfg->can_irq_callback(fifo);
	}
}



static rtems_device_driver satcan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t*)arg;
	int *value;
	rtems_interval *timeout;
	satcan_regmod *regmod;

	DBG("SatCAN:  IOCTL %d\n\r", ioarg->command);

	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
	case SATCAN_IOC_DMA_2K:
		DBG("SatCAN: ioctl: setting 2K DMA mode\n\r");
		free(priv->dmaptr);
		almalloc(&priv->alptr, &priv->dmaptr, ALIGN_2KMEM);
		if (priv->dmaptr == NULL) {
			printk("SatCAN: Failed to allocate DMA memory\n\r");
			return RTEMS_NO_MEMORY;
		}

		regs->membase = (unsigned int)priv->alptr;
		regs->satcan[SATCAN_RAM_BASE] = (unsigned int)priv->alptr >> OFFSET_2K_LOW_POS;
		regs->satcan[SATCAN_CMD1] = regs->satcan[SATCAN_CMD1] | Sel_2k_8kN;
		break;

	case SATCAN_IOC_DMA_8K:
		DBG("SatCAN: ioctl: setting 8K DMA mode\n\r");
		free(priv->dmaptr);
		almalloc(&priv->alptr, &priv->dmaptr, ALIGN_8KMEM);
		if (priv->dmaptr == NULL) {
			printk("SatCAN: Failed to allocate DMA memory\n\r");
			return RTEMS_NO_MEMORY;
		}
		
		regs->membase = (unsigned int)priv->alptr;
		regs->satcan[SATCAN_RAM_BASE] = (unsigned int)priv->alptr >> OFFSET_8K_LOW_POS;
		regs->satcan[SATCAN_CMD1] = regs->satcan[SATCAN_CMD1] & ~Sel_2k_8kN;
		break;
		
	case SATCAN_IOC_GET_REG:
		/* Get regmod structure from argument */
		regmod = (satcan_regmod*)ioarg->buffer;
		DBG("SatCAN: ioctl: getting register %d\n\r", regmod->reg);
		if (regmod->reg < 0)
			return RTEMS_INVALID_NAME;
		else if (regmod->reg <= SATCAN_FILTER_STOP)
			regmod->val = regs->satcan[regmod->reg];
		else if (regmod->reg == SATCAN_WCTRL)
			regmod->val = regs->ctrl;
		else if (regmod->reg == SATCAN_WIPEND)
			regmod->val = regs->irqpend;
		else if (regmod->reg == SATCAN_WIMASK)
			regmod->val = regs->irqmask;
		else if (regmod->reg == SATCAN_WAHBADDR)
			regmod->val = regs->membase;
		else
			return RTEMS_INVALID_NAME;
		break;

	case SATCAN_IOC_SET_REG:
		/* Get regmod structure from argument */
		regmod = (satcan_regmod*)ioarg->buffer;
		DBG("SatCAN: ioctl: setting register %d, value %x\n\r", 
		    regmod->reg, regmod->val);
		if (regmod->reg < 0)
			return RTEMS_INVALID_NAME;
		else if (regmod->reg <= SATCAN_FILTER_STOP)
			regs->satcan[regmod->reg] = regmod->val;
		else if (regmod->reg == SATCAN_WCTRL)
			regs->ctrl = regmod->val;
		else if (regmod->reg == SATCAN_WIPEND)
			regs->irqpend = regmod->val;
		else if (regmod->reg == SATCAN_WIMASK)
			regs->irqmask = regmod->val;
		else if (regmod->reg == SATCAN_WAHBADDR)
			regs->membase = regmod->val;
		else
			return RTEMS_INVALID_NAME;
		break;

	case SATCAN_IOC_OR_REG:
		/* Get regmod structure from argument */
		regmod = (satcan_regmod*)ioarg->buffer;
		DBG("SatCAN: ioctl: or:ing register %d, with value %x\n\r",
		    regmod->reg, regmod->val);
		if (regmod->reg < 0)
			return RTEMS_INVALID_NAME;
		else if (regmod->reg <= SATCAN_FILTER_STOP)
			regs->satcan[regmod->reg] |= regmod->val;
		else if (regmod->reg == SATCAN_WCTRL)
			regs->ctrl |= regmod->val;
		else if (regmod->reg == SATCAN_WIPEND)
			regs->irqpend |= regmod->val;
		else if (regmod->reg == SATCAN_WIMASK)
			regs->irqmask |= regmod->val;
		else if (regmod->reg == SATCAN_WAHBADDR)
			regs->membase |= regmod->val;
		else
			return RTEMS_INVALID_NAME;
		break;

	case SATCAN_IOC_AND_REG:
		/* Get regmod structure from argument */
		regmod = (satcan_regmod*)ioarg->buffer;
		DBG("SatCAN: ioctl: masking register %d, with value %x\n\r",
		    regmod->reg, regmod->val);
		if (regmod->reg < 0)
			return RTEMS_INVALID_NAME;
		else if (regmod->reg <= SATCAN_FILTER_STOP)
			regs->satcan[regmod->reg] &= regmod->val;
		else if (regmod->reg == SATCAN_WCTRL)
			regs->ctrl &= regmod->val;
		else if (regmod->reg == SATCAN_WIPEND)
			regs->irqpend &= regmod->val;
		else if (regmod->reg == SATCAN_WIMASK)
			regs->irqmask &= regmod->val;
		else if (regmod->reg == SATCAN_WAHBADDR)
			regs->membase &= regmod->val;
		else
			return RTEMS_INVALID_NAME;
		break;

	case SATCAN_IOC_EN_TX1_DIS_TX2:
		priv->dmaen = SATCAN_DMA_ENABLE_TX1;
		break;

	case SATCAN_IOC_EN_TX2_DIS_TX1:
		priv->dmaen = SATCAN_DMA_ENABLE_TX2;
		break;

	case SATCAN_IOC_GET_DMA_MODE:
		value = (int*)ioarg->buffer;
		*value = priv->dmamode;
		break;
		
	case SATCAN_IOC_SET_DMA_MODE:
		value = (int*)ioarg->buffer;
		if (*value != SATCAN_DMA_MODE_USER && *value != SATCAN_DMA_MODE_SYSTEM) {
			DBG("SatCAN: ioctl: invalid DMA mode\n\r");
			return RTEMS_INVALID_NAME;
		}
		priv->dmamode = *value;
		break;

	case SATCAN_IOC_ACTIVATE_DMA:
		if (priv->dmamode != SATCAN_DMA_MODE_USER) {
			DBG("SatCAN: ioctl: ACTIVATE_DMA: not in user mode\n\r");
			return RTEMS_INVALID_NAME;
		}
		value = (int*)ioarg->buffer;
		if (*value != SATCAN_DMA_ENABLE_TX1 && *value != SATCAN_DMA_ENABLE_TX2) {
			DBG("SatCAN: ioctl: ACTIVATE_DMA: Illegal channel\n\r");
			return RTEMS_INVALID_NAME;
		}
		regs->satcan[SATCAN_DMA] |= *value << 1;
		break;

	case SATCAN_IOC_DEACTIVATE_DMA:
		if (priv->dmamode != SATCAN_DMA_MODE_USER) {
			DBG("SatCAN: ioctl: DEACTIVATE_DMA: not in user mode\n\r");
			return RTEMS_INVALID_NAME;
		}
		value = (int*)ioarg->buffer;
		if (*value != SATCAN_DMA_ENABLE_TX1 && *value != SATCAN_DMA_ENABLE_TX2) {
			DBG("SatCAN: ioctl: DEACTIVATE_DMA: Illegal channel\n\r");
			return RTEMS_INVALID_NAME;
		}
		regs->satcan[SATCAN_DMA] &= ~(*value << 1);
		break;	

	case SATCAN_IOC_GET_DOFFSET:
		value = (int*)ioarg->buffer;
		*value = priv->doff;
		break;
	
	case SATCAN_IOC_SET_DOFFSET:
		value = (int*)ioarg->buffer;
		priv->doff = *value;
		break;
		
	case SATCAN_IOC_GET_TIMEOUT:
		timeout = (rtems_interval*)ioarg->buffer;
		*timeout = priv->timeout;
		break;

	case SATCAN_IOC_SET_TIMEOUT:
		timeout = (rtems_interval*)ioarg->buffer;
		priv->timeout = *timeout;
		break;

	default:
		return RTEMS_NOT_DEFINED;
	}

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver satcan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	int i;
	int doff;
	int msgindex;
	int messages;
	rtems_libio_rw_args_t *rw_args=(rtems_libio_rw_args_t *) arg;
	satcan_msg *msgs;
	rtems_status_code status;
	
	DBG("SatCAN: Writing %d bytes from %p\n\r",rw_args->count,rw_args->buffer);

	if ((rw_args->count < sizeof(satcan_msg)) || (!rw_args->buffer)) {
		DBG("SatCAN: write: returning EINVAL\n\r");
		return RTEMS_INVALID_NAME; /* EINVAL */
	}

	messages = rw_args->count / sizeof(satcan_msg);
	msgs = (satcan_msg*)rw_args->buffer;

	/* Check that size matches any number of satcan_msg */
	if (rw_args->count % sizeof(satcan_msg)) {
		DBG("SatCAN: write: count can not be evenly divided with satcan_msg size\n\r");
		return RTEMS_INVALID_NAME; /* EINVAL */
	}


	/* DMA channel must be set if we are in system DMA mode */
	DBG("SatCAN: write: dma channel select is %x\n\r", priv->dmaen);
	if (!priv->dmaen && priv->dmamode == SATCAN_DMA_MODE_SYSTEM)
		return RTEMS_INVALID_NAME; /* EINVAL */

	/* DMA must not be active */
	if (regs->satcan[SATCAN_DMA] & (DMA_EnTx1 | DMA_EnTx2 | DMA_AutoInitDmaTx)) {
		DBG("SatCAN: write: DMA was active\n\r");
		rw_args->bytes_moved = 0;
		return RTEMS_IO_ERROR; /* EIO */
	}		

	doff = regs->satcan[SATCAN_CMD1] & Sel_2k_8kN ? DMA_2K_DATA_OFFSET : DMA_8K_DATA_OFFSET;

	for (msgindex = 0; msgindex < messages; msgindex++) {
		/* Place header in DMA area */
		for (i = 0; i < SATCAN_HEADER_SIZE; i++) {
			priv->alptr[priv->doff+8*msgindex+i] = msgs[msgindex].header[i];
		}

		/* Place data in DMA area */
		for (i = 0; i < SATCAN_PAYLOAD_SIZE; i++)
			priv->alptr[priv->doff+doff+8*msgindex+i] = msgs[msgindex].payload[i];
	}

	if ((priv->dmaen & SATCAN_DMA_ENABLE_TX1) ||  priv->dmamode == SATCAN_DMA_MODE_USER) {
		regs->satcan[SATCAN_DMA_TX_1_CUR] = 0;
		regs->satcan[SATCAN_DMA_TX_1_END] = messages<<3; 
	}

	if ((priv->dmaen & SATCAN_DMA_ENABLE_TX2) || priv->dmamode == SATCAN_DMA_MODE_USER) {
		regs->satcan[SATCAN_DMA_TX_2_CUR] = 0;
		regs->satcan[SATCAN_DMA_TX_2_END] = messages<<3; 
	}

	/* If we are in DMA user mode we are done here, otherwise we block */
	if (priv->dmamode == SATCAN_DMA_MODE_SYSTEM) {
		priv->txactive = 1;

		/* Enable DMA */
		regs->satcan[SATCAN_DMA] |= priv->dmaen << 1;
		
		/* Wait for TX interrupt */
		status = rtems_semaphore_obtain(priv->txsem, RTEMS_WAIT, priv->timeout);
		
		priv->txactive = 0;

		/* Disable activated Tx DMA */
		regs->satcan[SATCAN_DMA] &= ~(priv->dmaen << 1);

		if (status != RTEMS_SUCCESSFUL) {
			rw_args->bytes_moved = 0;
			return status;
		}
	}

	rw_args->bytes_moved = rw_args->count;
	
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver satcan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	char *buf;
	int i;
	int canid;
	int messages;
	rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t*)arg;
	satcan_msg *ret;

	/* Check that there is room for the return */
	if (rw_args->count < sizeof(satcan_msg)) {
		DBG("SatCAN: read: length of buffer must be at least %d, current is %d\n\r",
		    sizeof(satcan_msg) + sizeof(int), rw_args->count);
		return RTEMS_INVALID_NAME; /* -EINVAL */
	}
	
	/* Check that size matches any number of satcan_msg */
	if (rw_args->count % sizeof(satcan_msg)) {
		DBG("SatCAN: read: count can not be evenly divided with satcan_msg size\n\r");
		return RTEMS_INVALID_NAME; /* EINVAL */
	}
	
	messages = rw_args->count / sizeof(satcan_msg);
	ret = (satcan_msg*)rw_args->buffer;

	DBG("SatCAN: read: reading %d messages to %p\n\r", messages, ret);
	
	for (i = 0; i < messages; i++) {
		canid = (ret[i].header[1] << 8) | ret[i].header[0];
	
		/* Copy message header from DMA header area to buffer */
		buf = (char*)((int)priv->alptr | (canid << 3));
		memcpy(ret[i].header, buf, SATCAN_HEADER_SIZE);

		DBG("SatCAN: read: copied header from %p to %p\n\r", buf, ret[i].header);

		/* Clear New Message Marker */
		buf[SATCAN_HEADER_NMM_POS] = 0;
		
		/* Copy message payload from DMA data area to buffer */
		buf = (char*)((int)buf | 
			      (regs->satcan[SATCAN_CMD1] & Sel_2k_8kN ? DMA_2K_DATA_SELECT : DMA_8K_DATA_SELECT));
		memcpy(ret[i].payload, buf, SATCAN_PAYLOAD_SIZE);
	
		DBG("SatCAN: read: copied payload from %p to %p\n\r", buf, ret[i].payload);
	}
	rw_args->bytes_moved = rw_args->count;

	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver satcan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	DBG("SatCAN: Closing %d\n\r",minor);

	if (priv->open) {
		regs->irqmask = 0;
		regs->satcan[SATCAN_INT_EN] = 0;
		regs->satcan[SATCAN_RX] = 0;
		regs->satcan[SATCAN_DMA] = 0;
		priv->open = 0;
		priv->dmaen = 0;
		priv->doff = 0;
		priv->timeout = RTEMS_NO_TIMEOUT;
		priv->dmamode = SATCAN_DMA_MODE_SYSTEM;
	}

	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver satcan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	DBG("SatCAN: Opening %d\n\r",minor);
	
	rtems_semaphore_obtain(priv->devsem,RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (priv->open) {
		rtems_semaphore_release(priv->devsem);
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */
	}
	priv->open = 1;
	rtems_semaphore_release(priv->devsem);

	/* Enable AHB and CAN IRQs in wrapper and EOD1, EOD2 and CAN critical  IRQs in SatCAN core */
	regs->irqmask = MSK_AHB | MSK_CAN;
	regs->satcan[SATCAN_INT_EN] = ((1 << SATCAN_IRQ_EOD1) | (1 << SATCAN_IRQ_EOD2) |
				       (1 << SATCAN_IRQ_CRITICAL));

	/* Select can_int as IRQ source */
	regs->satcan[SATCAN_CMD0] = CAN_TODn_Int_sel;
	/* CAN RX DMA Enable */
	regs->satcan[SATCAN_DMA] = 1;
	/* CAN RX Enable */
	regs->satcan[SATCAN_RX] = 1;

	DBG("SatCAN: Opening %d success\n\r",minor);

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver satcan_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct ambapp_ahb_info d;
	char fs_name[20];
	rtems_status_code status;

	DBG("SatCAN: Initialize..\n\r");

	strcpy(fs_name, SATCAN_DEVNAME);

	/* Find core and initialize register pointer */
	if (!ambapp_find_ahbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_SATCAN, &d)) {
		printk("SatCAN: Failed to find SatCAN core\n\r");
		return -1;
	}

	status = rtems_io_register_name(fs_name, major, minor);
	if (RTEMS_SUCCESSFUL != status)
		rtems_fatal_error_occurred(status);

	regs = (struct satcan_regs*)d.start[0];
		
	/* Set node number and DPS */
	regs->ctrl |= ((priv->cfg->nodeno & 0xf) << 5) | (priv->cfg->dps << 1);

	/* Reset core */
	regs->ctrl |= CTRL_RST;

	/* Allocate DMA area */
	almalloc(&priv->alptr, &priv->dmaptr, ALIGN_2KMEM);
	if (priv->dmaptr == NULL) {
		printk("SatCAN: Failed to allocate DMA memory\n\r");
		free(priv->cfg);
		free(priv);
		return -1; 
	}
	
	/* Wait until core reset has completed */
	while (regs->ctrl & CTRL_RST)
		;

	/* Initialize core registers, default is 2K messages */
	regs->membase = (unsigned int)priv->alptr;
	regs->satcan[SATCAN_RAM_BASE] = (unsigned int)priv->alptr >> 15;
	
	DBG("regs->membase = %x\n\r", (unsigned int)priv->alptr);
	DBG("regs->satcan[SATCAN_RAM_BASE] = %x\n\r", (unsigned int)priv->alptr >> 15);

	status = rtems_semaphore_create(
		rtems_build_name('S', 'd', 'v', '0'),
		1,
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
		RTEMS_NO_PRIORITY_CEILING,
		0, 
		&priv->devsem);
	if (status != RTEMS_SUCCESSFUL) {
		printk("SatCAN: Failed to create dev semaphore (%d)\n\r", status);
		free(priv->cfg);
		free(priv);
		return RTEMS_UNSATISFIED;
	}
	status = rtems_semaphore_create(
		rtems_build_name('S', 't', 'x', '0'),
		0, 
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
		RTEMS_NO_PRIORITY_CEILING,
		0, 
		&priv->txsem);
	if (status != RTEMS_SUCCESSFUL) {
	  printk("SatCAN: Failed to create tx semaphore (%d)\n\r", status);
	  free(priv->cfg);
	  free(priv);
	  return RTEMS_UNSATISFIED;
	}

	priv->txactive = 0;
	priv->open = 0;
	priv->dmaen = 0;
	priv->doff = 0;
	priv->timeout = RTEMS_NO_TIMEOUT;
	priv->dmamode = SATCAN_DMA_MODE_SYSTEM;

	/* Register interrupt handler */
	rtems_interrupt_handler_install(d.irq, "satcan",
			RTEMS_INTERRUPT_SHARED,
			satcan_interrupt_handler, NULL);

	return RTEMS_SUCCESSFUL;
}



#define SATCAN_DRIVER_TABLE_ENTRY { satcan_initialize, satcan_open, satcan_close, satcan_read, satcan_write, satcan_ioctl }

static rtems_driver_address_table satcan_driver = SATCAN_DRIVER_TABLE_ENTRY;

int satcan_register(satcan_config *conf)
{
	rtems_status_code r;
	rtems_device_major_number m;

	DBG("SatCAN: satcan_register called\n\r");

	/* Create private structure */
	if ((priv = grlib_malloc(sizeof(*priv))) == NULL) {
		printk("SatCAN driver could not allocate memory for priv structure\n\r");
		return -1;
	}

	DBG("SatCAN: Creating local copy of config structure\n\r");
	if ((priv->cfg = grlib_malloc(sizeof(*priv->cfg))) == NULL) {
		printk("SatCAN driver could not allocate memory for cfg structure\n\r");
		return 1;
	}
	memcpy(priv->cfg, conf, sizeof(satcan_config));

	if ((r = rtems_io_register_driver(0, &satcan_driver, &m)) == RTEMS_SUCCESSFUL) {
		DBG("SatCAN driver successfully registered, major: %d\n\r", m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("SatCAN rtems_io_register_driver failed: RTEMS_TOO_MANY\n\r"); break;
		case RTEMS_INVALID_NUMBER:  
			printk("SatCAN rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n\r"); break;
		case RTEMS_RESOURCE_IN_USE:
			printk("SatCAN rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n\r"); break;
		default:
			printk("SatCAN rtems_io_register_driver failed\n\r");
		}
		return 1;
	}

	return 0;
}
