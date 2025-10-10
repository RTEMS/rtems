/* SPDX-License-Identifier: BSD-2-Clause */

/*  OC_CAN driver
 *
 *  COPYRIGHT (c) 2007.
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

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bsp.h>
#include <rtems/bspIo.h> /* printk */

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/occan.h>
#include <grlib/canbtrs.h>

#include <grlib/grlib_impl.h>

/* RTEMS -> ERRNO decoding table

rtems_assoc_t errno_assoc[] = {
    { "OK",                 RTEMS_SUCCESSFUL,                0 },
    { "BUSY",               RTEMS_RESOURCE_IN_USE,           EBUSY },
    { "INVALID NAME",       RTEMS_INVALID_NAME,              EINVAL },
    { "NOT IMPLEMENTED",    RTEMS_NOT_IMPLEMENTED,           ENOSYS },
    { "TIMEOUT",            RTEMS_TIMEOUT,                   ETIMEDOUT },
    { "NO MEMORY",          RTEMS_NO_MEMORY,                 ENOMEM },
    { "NO DEVICE",          RTEMS_UNSATISFIED,               ENODEV },
    { "INVALID NUMBER",     RTEMS_INVALID_NUMBER,            EBADF},
    { "NOT RESOURCE OWNER", RTEMS_NOT_OWNER_OF_RESOURCE,     EPERM},
    { "IO ERROR",           RTEMS_IO_ERROR,                  EIO},
    { 0, 0, 0 },
};

*/

/*
#undef DEBUG
#undef DEBUG_EXTRA
#undef DEBUG_PRINT_REGMAP
*/

/* default to byte regs */
#ifndef OCCAN_WORD_REGS
 #define OCCAN_BYTE_REGS
#else
 #undef OCCAN_BYTE_REGS
#endif

/* Enable Fixup code older OCCAN with a TX IRQ-FLAG bug */
#define OCCAN_TX_IRQ_FLAG_FIXUP 1

#define OCCAN_WORD_REG_OFS 0x80
#define OCCAN_NCORE_OFS 0x100
#define DEFAULT_CLKDIV 0x7
#define DEFAULT_EXTENDED_MODE 1
#define DEFAULT_RX_FIFO_LEN 64
#define DEFAULT_TX_FIFO_LEN 64

/* not implemented yet */
#undef REDUNDANT_CHANNELS

/* Define common debug macros */
#ifdef DEBUG
	#define DBG(fmt, vargs...) printk(fmt, ## vargs )
#else
	#define DBG(fmt, vargs...)
#endif

/* fifo interface */
typedef struct {
	int cnt;
	int ovcnt; /* overwrite count */
	int full; /* 1 = base contain cnt CANMsgs, tail==head */
	CANMsg *tail, *head;
	CANMsg *base;
	CANMsg fifoarea[0];
} occan_fifo;

/* PELICAN */

typedef struct {
	unsigned char
		mode,
		cmd,
		status,
		intflags,
		inten,
		resv0,
		bustim0,
		bustim1,
		unused0[2],
		resv1,
		arbcode,
		errcode,
		errwarn,
		rx_err_cnt,
		tx_err_cnt,
		rx_fi_xff; /* this is also acceptance code 0 in reset mode */
		union{
			struct {
				unsigned char id[2];
				unsigned char data[8];
				unsigned char next_in_fifo[2];
			} rx_sff;
			struct {
				unsigned char id[4];
				unsigned char data[8];
			} rx_eff;
			struct {
				unsigned char id[2];
				unsigned char data[8];
				unsigned char unused[2];
			} tx_sff;
			struct {
				unsigned char id[4];
				unsigned char data[8];
			} tx_eff;
			struct {
				unsigned char code[3];
				unsigned char mask[4];
			} rst_accept;
		} msg;
		unsigned char rx_msg_cnt;
		unsigned char unused1;
		unsigned char clkdiv;
} pelican8_regs;

typedef struct {
	unsigned char
		mode, unused0[3],
		cmd, unused1[3],
		status, unused2[3],
		intflags, unused3[3],
		inten, unused4[3],
		resv0, unused5[3],
		bustim0, unused6[3],
		bustim1, unused7[3],
		unused8[8],
		resv1,unused9[3],
		arbcode,unused10[3],
		errcode,unused11[3],
		errwarn,unused12[3],
		rx_err_cnt,unused13[3],
		tx_err_cnt,unused14[3],
		rx_fi_xff, unused15[3]; /* this is also acceptance code 0 in reset mode */
		/* make sure to use pointers when writing (byte access) to these registers */
		union{
			struct {
				unsigned int id[2];
				unsigned int data[8];
				unsigned int next_in_fifo[2];
			} rx_sff;
			struct {
				unsigned int id[4];
				unsigned int data[8];
			} rx_eff;
			struct {
				unsigned int id[2];
				unsigned int data[8];
			} tx_sff;
			struct {
				unsigned int id[4];
				unsigned int data[8];
			} tx_eff;
			struct {
				unsigned int code[3];
				unsigned int mask[4];
			} rst_accept;
		} msg;
		unsigned char rx_msg_cnt,unused16[3];
		unsigned char unused17[4];
		unsigned char clkdiv,unused18[3];
} pelican32_regs;

#ifdef OCCAN_BYTE_REGS
#define pelican_regs pelican8_regs
#else
#define pelican_regs pelican32_regs
#endif

/* Default sampling point in % */
#define OCCAN_SAMPLING_POINT 90

/* OCCAN baud-rate paramter boundaries */
struct grlib_canbtrs_ranges occan_btrs_ranges = {
	.max_scaler = 64,
	.has_bpr = 0,
	.divfactor = 1,
	.min_tseg1 = 1,
	.max_tseg1 = 16,
	.min_tseg2 = 1,
	.max_tseg2 = 8,
};

typedef struct {
	unsigned char btr0;
	unsigned char btr1;
} occan_speed_regs;

typedef struct {
	struct drvmgr_dev *dev;
	char devName[52];
	SPIN_DECLARE(devlock);

	/* hardware shortcuts */
	pelican_regs *regs;
	int byte_regs;
	int irq;
	occan_speed_regs timing;
	int channel; /* 0=default, 1=second bus */
	int single_mode;
	unsigned int sys_freq_hz;

	/* driver state */
	rtems_id devsem;
	rtems_id txsem;
	rtems_id rxsem;
	int open;
	int started;
	int rxblk;
	int txblk;
	int sending;
	unsigned int status;
	occan_stats stats;

	/* rx&tx fifos */
	occan_fifo *rxfifo;
	occan_fifo *txfifo;

	/* Config */
	unsigned int speed; /* speed in HZ */
	unsigned char acode[4];
	unsigned char amask[4];
} occan_priv;

/********** FIFO INTERFACE **********/
static void occan_fifo_put(occan_fifo *fifo);
static CANMsg *occan_fifo_put_claim(occan_fifo *fifo, int force);
static occan_fifo *occan_fifo_create(int cnt);
static void occan_fifo_free(occan_fifo *fifo);
static int occan_fifo_full(occan_fifo *fifo);
static int occan_fifo_empty(occan_fifo *fifo);
static void occan_fifo_get(occan_fifo *fifo);
static CANMsg *occan_fifo_claim_get(occan_fifo *fifo);
static void occan_fifo_clr(occan_fifo *fifo);

/**** Hardware related Interface ****/
static void convert_timing_to_btrs(
	struct grlib_canbtrs_timing *t,
	occan_speed_regs *btrs);
static int occan_set_speedregs(occan_priv *priv, occan_speed_regs *timing);
static void pelican_init(occan_priv *priv);
static void pelican_open(occan_priv *priv);
static int pelican_start(occan_priv *priv);
static void pelican_stop(occan_priv *priv);
static int pelican_send(occan_priv *can, CANMsg *msg);
static void pelican_set_accept(occan_priv *priv, unsigned char *acode, unsigned char *amask);
void occan_interrupt(void *arg);
#ifdef DEBUG_PRINT_REGMAP
static void pelican_regadr_print(pelican_regs *regs);
#endif

/***** Driver related interface *****/
static rtems_device_driver occan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver occan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver occan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver occan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver occan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver occan_initialize(rtems_device_major_number major, rtems_device_minor_number unused, void *arg);

#define OCCAN_DRIVER_TABLE_ENTRY { occan_initialize, occan_open, occan_close, occan_read, occan_write, occan_ioctl }
static rtems_driver_address_table occan_driver = OCCAN_DRIVER_TABLE_ENTRY;


/* Read byte bypassing */


/* Bypass cache */
#define READ_REG(priv, address) occan_reg_read(priv, (unsigned int)address)
#define WRITE_REG(priv, address, data) occan_reg_write(priv, (unsigned int)address, data)

static unsigned int occan_reg_read(occan_priv *priv, unsigned int address)
{
	unsigned int adr;
	if ( priv->byte_regs ) {
		adr = address;
	} else {
		/* Word accessed registers */
		adr = (address & (~0x7f)) | ((address & 0x7f)<<2);
	}
	return *(volatile unsigned char *)adr;
}

static void occan_reg_write(
	occan_priv *priv, 
	unsigned int address,
	unsigned char value)
{
	unsigned int adr;
	if ( priv->byte_regs ) {
		adr = address;
	} else {
		/* Word accessed registers */
		adr = (address & (~0x7f)) | ((address & 0x7f)<<2);
	}
	*(volatile unsigned char *)adr = value;;
}

/* Mode register bit definitions */
#define PELICAN_MOD_RESET          0x1
#define PELICAN_MOD_LISTEN         0x2
#define PELICAN_MOD_SELFTEST       0x4
#define PELICAN_MOD_ACCEPT         0x8

/* Command register bit definitions */
#define PELICAN_CMD_TXREQ          0x1
#define PELICAN_CMD_ABORT          0x2
#define PELICAN_CMD_RELRXBUF       0x4
#define PELICAN_CMD_CLRDOVR        0x8
#define PELICAN_CMD_SELFRXRQ       0x10

/* Status register bit definitions */
#define PELICAN_STAT_RXBUF         0x1
#define PELICAN_STAT_DOVR          0x2
#define PELICAN_STAT_TXBUF         0x4
#define PELICAN_STAT_TXOK          0x8
#define PELICAN_STAT_RX            0x10
#define PELICAN_STAT_TX            0x20
#define PELICAN_STAT_ERR           0x40
#define PELICAN_STAT_BUS           0x80

/* Interrupt register bit definitions */
#define PELICAN_IF_RX         0x1
#define PELICAN_IF_TX         0x2
#define PELICAN_IF_ERRW       0x4
#define PELICAN_IF_DOVR       0x8
#define PELICAN_IF_ERRP       0x20
#define PELICAN_IF_ARB        0x40
#define PELICAN_IF_BUS        0x80

/* Interrupt Enable register bit definitions */
#define PELICAN_IE_RX         0x1
#define PELICAN_IE_TX         0x2
#define PELICAN_IE_ERRW       0x4
#define PELICAN_IE_DOVR       0x8
#define PELICAN_IE_ERRP       0x20
#define PELICAN_IE_ARB        0x40
#define PELICAN_IE_BUS        0x80

/* Arbitration lost capture register bit definitions */
#define PELICAN_ARB_BITS      0x1f

/*  register bit definitions */
#define PELICAN_ECC_CODE_BIT     0x00
#define PELICAN_ECC_CODE_FORM    0x40
#define PELICAN_ECC_CODE_STUFF   0x80
#define PELICAN_ECC_CODE_OTHER   0xc0
#define PELICAN_ECC_CODE    0xc0

#define PELICAN_ECC_DIR     0x20
#define PELICAN_ECC_SEG     0x1f

/* Clock divider register bit definitions */
#define PELICAN_CDR_DIV  0x7
#define PELICAN_CDR_OFF  0x8
#define PELICAN_CDR_MODE 0x80
#define PELICAN_CDR_MODE_PELICAN 0x80
#define PELICAN_CDR_MODE_BITS 7
#define PELICAN_CDR_MODE_BASICAN 0x00


/*  register bit definitions */
#define OCCAN_BUSTIM_SJW       0xc0
#define OCCAN_BUSTIM_BRP       0x3f
#define OCCAN_BUSTIM_SJW_BIT   6

#define OCCAN_BUSTIM_SAM       0x80
#define OCCAN_BUSTIM_TSEG2     0x70
#define OCCAN_BUSTIM_TSEG2_BIT 4
#define OCCAN_BUSTIM_TSEG1     0x0f

/*  register bit definitions */
/*
#define PELICAN_S_       0x1
#define PELICAN_S_       0x2
#define PELICAN_S_       0x4
#define PELICAN_S_       0x8
#define PELICAN_S_       0x10
#define PELICAN_S_       0x20
#define PELICAN_S_       0x40
#define PELICAN_S_       0x80
*/

static int occan_driver_io_registered = 0;
static rtems_device_major_number occan_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
int occan_register_io(rtems_device_major_number *m);
int occan_device_init(occan_priv *pDev);

int occan_init2(struct drvmgr_dev *dev);
int occan_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops occan_ops = 
{
	.init = {NULL, occan_init2, occan_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id occan_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_CANAHB},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info occan_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_OCCAN_ID,	/* Driver ID */
		"OCCAN_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&occan_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&occan_ids[0]
};

void occan_register_drv (void)
{
	DBG("Registering OCCAN driver\n");
	drvmgr_drv_register(&occan_drv_info.general);
}

int occan_init2(struct drvmgr_dev *dev)
{
	occan_priv *priv;

	DBG("OCCAN[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	return DRVMGR_OK;
}

int occan_init3(struct drvmgr_dev *dev)
{
	occan_priv *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( occan_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( occan_register_io(&occan_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		occan_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */

	if ( occan_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/occan%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%soccan%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	DBG("OCCAN[%d]: Registering %s\n", dev->minor_drv, priv->devName);
	status = rtems_io_register_name(priv->devName, occan_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

int occan_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &occan_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("OCCAN driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("OCCAN rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("OCCAN rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("OCCAN rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("OCCAN rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

int occan_device_init(occan_priv *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	rtems_status_code status;
	int minor;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irq = pnpinfo->irq;
	pDev->regs = (pelican_regs *)(pnpinfo->ahb_slv->start[0] + OCCAN_NCORE_OFS*pnpinfo->index);
	pDev->byte_regs = 1;
	minor = pDev->dev->minor_drv;

	/* Get frequency in Hz */
	if ( drvmgr_freq_get(pDev->dev, DEV_AHB_SLV, &pDev->sys_freq_hz) ) {
		return -1;
	}

	DBG("OCCAN frequency: %d Hz\n", pDev->sys_freq_hz);

	/* initialize software */
	pDev->open = 0;
	pDev->started = 0; /* Needed for spurious interrupts */
	pDev->rxfifo = NULL;
	pDev->txfifo = NULL;
	status = rtems_semaphore_create(
			rtems_build_name('C', 'd', 'v', '0'+minor),
			1,
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
			RTEMS_NO_PRIORITY_CEILING,
			0, 
			&pDev->devsem);
	if ( status != RTEMS_SUCCESSFUL ){
		printk("OCCAN[%d]: Failed to create dev semaphore, (%d)\n\r",minor, status);
		return RTEMS_UNSATISFIED;
	}
	status = rtems_semaphore_create(
			rtems_build_name('C', 't', 'x', '0'+minor),
			0, 
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
			RTEMS_NO_PRIORITY_CEILING,
                        0, 
			&pDev->txsem);
	if ( status != RTEMS_SUCCESSFUL ){
		printk("OCCAN[%d]: Failed to create tx semaphore, (%d)\n\r",minor, status);
		return RTEMS_UNSATISFIED;
	}
	status = rtems_semaphore_create(
			rtems_build_name('C', 'r', 'x', '0'+minor),
			0, 
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
			RTEMS_NO_PRIORITY_CEILING,
			0,
			&pDev->rxsem);
	if ( status != RTEMS_SUCCESSFUL ){
		printk("OCCAN[%d]: Failed to create rx semaphore, (%d)\n\r",minor, status);
		return RTEMS_UNSATISFIED;
	}

	/* hardware init/reset */
	pelican_init(pDev);

#ifdef DEBUG_PRINT_REGMAP
	pelican_regadr_print(pDev->regs);
#endif

	return 0;
}


#ifdef DEBUG
static void pelican_regs_print(occan_priv *pDev){
	pelican_regs *regs = pDev->regs;
	printk("--- PELICAN 0x%lx ---\n\r",(unsigned int)regs);
	printk(" MODE: 0x%02x\n\r",READ_REG(pDev, &regs->mode));
	printk(" CMD: 0x%02x\n\r",READ_REG(pDev, &regs->cmd));
	printk(" STATUS: 0x%02x\n\r",READ_REG(pDev, &regs->status));
	/*printk(" INTFLG: 0x%02x\n\r",READ_REG(pDev, &regs->intflags));*/
	printk(" INTEN: 0x%02x\n\r",READ_REG(pDev, &regs->inten));
	printk(" BTR0: 0x%02x\n\r",READ_REG(pDev, &regs->bustim0));
	printk(" BTR1: 0x%02x\n\r",READ_REG(pDev, &regs->bustim1));
	printk(" ARBCODE: 0x%02x\n\r",READ_REG(pDev, &regs->arbcode));
	printk(" ERRCODE: 0x%02x\n\r",READ_REG(pDev, &regs->errcode));
	printk(" ERRWARN: 0x%02x\n\r",READ_REG(pDev, &regs->errwarn));
	printk(" RX_ERR_CNT: 0x%02x\n\r",READ_REG(pDev, &regs->rx_err_cnt));
	printk(" TX_ERR_CNT: 0x%02x\n\r",READ_REG(pDev, &regs->tx_err_cnt));
	if ( READ_REG(pDev, &regs->mode) & PELICAN_MOD_RESET ){
		/* in reset mode it is possible to read acceptance filters */
		printk(" ACR0: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->rx_fi_xff),&regs->rx_fi_xff);
		printk(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.code[0]),(unsigned int)&regs->msg.rst_accept.code[0]);
		printk(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.code[1]),(unsigned int)&regs->msg.rst_accept.code[1]);
		printk(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.code[2]),(unsigned int)&regs->msg.rst_accept.code[2]);
		printk(" AMR0: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.mask[0]),(unsigned int)&regs->msg.rst_accept.mask[0]);
		printk(" AMR1: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.mask[1]),(unsigned int)&regs->msg.rst_accept.mask[1]);
		printk(" AMR2: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.mask[2]),(unsigned int)&regs->msg.rst_accept.mask[2]);
		printk(" AMR3: 0x%02x (0x%lx)\n\r",READ_REG(pDev, &regs->msg.rst_accept.mask[3]),(unsigned int)&regs->msg.rst_accept.mask[3]);
		
	}else{
		printk(" RXFI_XFF: 0x%02x\n\r",READ_REG(pDev, &regs->rx_fi_xff));
	}
	printk(" RX_MSG_CNT: 0x%02x\n\r",READ_REG(pDev, &regs->rx_msg_cnt));
	printk(" CLKDIV: 0x%02x\n\r",READ_REG(pDev, &regs->clkdiv));
	printk("-------------------\n\r");
}
#endif

#ifdef DEBUG_PRINT_REGMAP
static void pelican_regadr_print(pelican_regs *regs){
	printk("--- PELICAN 0x%lx ---\n\r",(unsigned int)regs);
	printk(" MODE: 0x%lx\n\r",(unsigned int)&regs->mode);
	printk(" CMD: 0x%lx\n\r",(unsigned int)&regs->cmd);
	printk(" STATUS: 0x%lx\n\r",(unsigned int)&regs->status);
	/*printk(" INTFLG: 0x%lx\n\r",&regs->intflags);*/
	printk(" INTEN: 0x%lx\n\r",(unsigned int)&regs->inten);
	printk(" BTR0: 0x%lx\n\r",(unsigned int)&regs->bustim0);
	printk(" BTR1: 0x%lx\n\r",(unsigned int)&regs->bustim1);
	printk(" ARBCODE: 0x%lx\n\r",(unsigned int)&regs->arbcode);
	printk(" ERRCODE: 0x%lx\n\r",(unsigned int)&regs->errcode);
	printk(" ERRWARN: 0x%lx\n\r",(unsigned int)&regs->errwarn);
	printk(" RX_ERR_CNT: 0x%lx\n\r",(unsigned int)&regs->rx_err_cnt);
	printk(" TX_ERR_CNT: 0x%lx\n\r",(unsigned int)&regs->tx_err_cnt);

	/* in reset mode it is possible to read acceptance filters */
	printk(" RXFI_XFF: 0x%lx\n\r",(unsigned int)&regs->rx_fi_xff);
	
	/* reset registers */
	printk(" ACR0: 0x%lx\n\r",(unsigned int)&regs->rx_fi_xff);
	printk(" ACR1: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[0]);
	printk(" ACR2: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[1]);
	printk(" ACR3: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[2]);
	printk(" AMR0: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[0]);
	printk(" AMR1: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[1]);
	printk(" AMR2: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[2]);
	printk(" AMR3: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[3]);
	
	/* TX Extended */
	printk(" EFFTX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[0]);
	printk(" EFFTX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[1]);
	printk(" EFFTX_ID[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[2]);
	printk(" EFFTX_ID[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[3]);

	printk(" EFFTX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[0]);
	printk(" EFFTX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[1]);
	printk(" EFFTX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[2]);	
	printk(" EFFTX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[3]);	
	printk(" EFFTX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[4]);	
	printk(" EFFTX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[5]);	
	printk(" EFFTX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[6]);	
	printk(" EFFTX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[7]);	

	/* RX Extended */
	printk(" EFFRX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[0]);
	printk(" EFFRX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[1]);
	printk(" EFFRX_ID[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[2]);
	printk(" EFFRX_ID[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[3]);

	printk(" EFFRX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[0]);
	printk(" EFFRX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[1]);
	printk(" EFFRX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[2]);	
	printk(" EFFRX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[3]);	
	printk(" EFFRX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[4]);	
	printk(" EFFRX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[5]);	
	printk(" EFFRX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[6]);	
	printk(" EFFRX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[7]);	


	/* RX Extended */
	printk(" SFFRX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.id[0]);
	printk(" SFFRX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.id[1]);

	printk(" SFFRX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[0]);
	printk(" SFFRX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[1]);
	printk(" SFFRX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[2]);	
	printk(" SFFRX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[3]);	
	printk(" SFFRX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[4]);	
	printk(" SFFRX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[5]);	
	printk(" SFFRX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[6]);	
	printk(" SFFRX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[7]);	

	/* TX Extended */
	printk(" SFFTX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.id[0]);
	printk(" SFFTX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.id[1]);

	printk(" SFFTX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[0]);
	printk(" SFFTX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[1]);
	printk(" SFFTX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[2]);
	printk(" SFFTX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[3]);
	printk(" SFFTX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[4]);
	printk(" SFFTX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[5]);
	printk(" SFFTX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[6]);
	printk(" SFFTX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[7]);

	printk(" RX_MSG_CNT: 0x%lx\n\r",(unsigned int)&regs->rx_msg_cnt);
	printk(" CLKDIV: 0x%lx\n\r",(unsigned int)&regs->clkdiv);
	printk("-------------------\n\r");
}
#endif

#ifdef DEBUG
static void occan_stat_print(occan_stats *stats){
	printk("----Stats----\n\r");
	printk("rx_msgs: %d\n\r",stats->rx_msgs);
	printk("tx_msgs: %d\n\r",stats->tx_msgs);
	printk("err_warn: %d\n\r",stats->err_warn);
	printk("err_dovr: %d\n\r",stats->err_dovr);
	printk("err_errp: %d\n\r",stats->err_errp);
	printk("err_arb: %d\n\r",stats->err_arb);
	printk("err_bus: %d\n\r",stats->err_bus);
	printk("Int cnt: %d\n\r",stats->ints);
	printk("tx_buf_err: %d\n\r",stats->tx_buf_error);
	printk("-------------\n\r");
}
#endif

static void pelican_init(occan_priv *priv){
	/* Reset core */
	WRITE_REG(priv, &priv->regs->mode, PELICAN_MOD_RESET);

	/* wait for core to reset complete */
	/*usleep(1);*/
}

static void pelican_open(occan_priv *priv){
	int ret;
	struct grlib_canbtrs_timing timing;

	/* Set defaults */
	priv->speed = OCCAN_SPEED_250K;

	/* set acceptance filters to accept all messages */
	priv->acode[0] = 0;
	priv->acode[1] = 0;
	priv->acode[2] = 0;
	priv->acode[3] = 0;
	priv->amask[0] = 0xff;
	priv->amask[1] = 0xff;
	priv->amask[2] = 0xff;
	priv->amask[3] = 0xff;

	/* Set clock divider to extended mode, clkdiv not connected
	 */
	WRITE_REG(priv, &priv->regs->clkdiv, (1<<PELICAN_CDR_MODE_BITS) | (DEFAULT_CLKDIV & PELICAN_CDR_DIV));

	ret = grlib_canbtrs_calc_timing(
			priv->speed, priv->sys_freq_hz,
			OCCAN_SAMPLING_POINT, &occan_btrs_ranges,
			(struct grlib_canbtrs_timing *)&timing);
	if ( ret ) {
		/* failed to set speed for this system freq, try with 50K instead */
		priv->speed = OCCAN_SPEED_50K;
		grlib_canbtrs_calc_timing(
			priv->speed, priv->sys_freq_hz,
			OCCAN_SAMPLING_POINT, &occan_btrs_ranges,
			(struct grlib_canbtrs_timing *)&timing);
	}
	convert_timing_to_btrs(&timing, &priv->timing);

	/* disable all interrupts */
	WRITE_REG(priv, &priv->regs->inten, 0);

	/* clear pending interrupts by reading */
	READ_REG(priv, &priv->regs->intflags);
}

static int pelican_start(occan_priv *priv){
	/* Start HW communication */

	if ( !priv->rxfifo || !priv->txfifo )
		return -1;

	/* In case we were started before and stopped we
	 * should empty the TX fifo or try to resend those
	 * messages. We make it simple...
	 */
	occan_fifo_clr(priv->txfifo);

	/* Clear status bits */
	priv->status = 0;
	priv->sending = 0;

	/* clear pending interrupts */
	READ_REG(priv, &priv->regs->intflags);

	/* clear error counters */
	WRITE_REG(priv, &priv->regs->rx_err_cnt, 0);
	WRITE_REG(priv, &priv->regs->tx_err_cnt, 0);

#ifdef REDUNDANT_CHANNELS
	if ( (priv->channel == 0) || (priv->channel >= REDUNDANT_CHANNELS) ){
		/* Select the first (default) channel */
		OCCAN_SET_CHANNEL(priv,0);
	}else{
		/* set gpio bit, or something */
		OCCAN_SET_CHANNEL(priv,priv->channel);
	}
#endif
	/* set the speed regs of the CAN core */
	occan_set_speedregs(priv,&priv->timing);

	DBG("OCCAN: start: set timing regs btr0: 0x%x, btr1: 0x%x\n\r",
		READ_REG(priv, &priv->regs->bustim0),
		READ_REG(priv, &priv->regs->bustim1));

	/* Set default acceptance filter */
	pelican_set_accept(priv,priv->acode,priv->amask);

	/* Nothing can fail from here, this must be set before interrupts are
	 * enabled */
	priv->started = 1;

	/* turn on interrupts */
	WRITE_REG(priv, &priv->regs->inten,
		PELICAN_IE_RX | PELICAN_IE_TX | PELICAN_IE_ERRW |
		PELICAN_IE_ERRP | PELICAN_IE_BUS);
#ifdef DEBUG
	/* print setup before starting */
	pelican_regs_print(priv->regs);
	occan_stat_print(&priv->stats);
#endif

	/* core already in reset mode,
	 *  * Exit reset mode
	 *  * Enter Single/Dual mode filtering.
	 */
	WRITE_REG(priv, &priv->regs->mode, (priv->single_mode << 3));

	/* Register interrupt routine and unmask IRQ at IRQ controller */
	drvmgr_interrupt_register(priv->dev, 0, "occan", occan_interrupt, priv);

	return 0;
}

static void pelican_stop(occan_priv *priv)
{
	/* stop HW */

	drvmgr_interrupt_unregister(priv->dev, 0, occan_interrupt, priv);

#ifdef DEBUG
	/* print setup before stopping */
	pelican_regs_print(priv->regs);
	occan_stat_print(&priv->stats);
#endif

	/* put core in reset mode */
	WRITE_REG(priv, &priv->regs->mode, PELICAN_MOD_RESET);

	/* turn off interrupts */
	WRITE_REG(priv, &priv->regs->inten, 0);

	priv->status |= OCCAN_STATUS_RESET;
}

static inline int pelican_tx_ready(occan_priv *can)
{
	unsigned char status;
	pelican_regs *regs = can->regs;

	/* is there room in send buffer? */
	status = READ_REG(can, &regs->status);
	if ( !(status & PELICAN_STAT_TXBUF) ) {
		/* tx fifo taken, we have to wait */
		return 0;
	}

	return 1;
}

/* Try to send message "msg", if hardware txfifo is
 * full, then -1 is returned.
 *
 * Be sure to have disabled CAN interrupts when
 * entering this function.
 */
static int pelican_send(occan_priv *can, CANMsg *msg){
	unsigned char tmp;
	pelican_regs *regs = can->regs;

	/* is there room in send buffer? */
	if ( !pelican_tx_ready(can) ) {
		/* tx fifo taken, we have to wait */
		return -1;
	}

	tmp = msg->len & 0xf;
	if ( msg->rtr )
		tmp |= 0x40;

	if ( msg->extended ){
		/* Extended Frame */
		WRITE_REG(can, &regs->rx_fi_xff, 0x80 | tmp);
		WRITE_REG(can, &regs->msg.tx_eff.id[0],(msg->id >> (5+8+8)) & 0xff);
		WRITE_REG(can, &regs->msg.tx_eff.id[1],(msg->id >> (5+8)) & 0xff);
		WRITE_REG(can, &regs->msg.tx_eff.id[2],(msg->id >> (5)) & 0xff);
		WRITE_REG(can, &regs->msg.tx_eff.id[3],(msg->id << 3) & 0xf8);
		tmp = msg->len;
		while(tmp--){
			WRITE_REG(can, &regs->msg.tx_eff.data[tmp], msg->data[tmp]);
		}
	}else{
		/* Standard Frame */
		WRITE_REG(can, &regs->rx_fi_xff, tmp);
		WRITE_REG(can, &regs->msg.tx_sff.id[0],(msg->id >> 3) & 0xff);
		WRITE_REG(can, &regs->msg.tx_sff.id[1],(msg->id << 5) & 0xe0);
		tmp = msg->len;
		while(tmp--){
			WRITE_REG(can, &regs->msg.tx_sff.data[tmp],msg->data[tmp]);
		}
	}

	/* let HW know of new message */
	if ( msg->sshot ){
		WRITE_REG(can, &regs->cmd, PELICAN_CMD_TXREQ | PELICAN_CMD_ABORT);
	}else{
		/* normal case -- try resend until sent */
		WRITE_REG(can, &regs->cmd, PELICAN_CMD_TXREQ);
	}

	return 0;
}


static void pelican_set_accept(occan_priv *priv, unsigned char *acode, unsigned char *amask)
{
	unsigned char *acode0, *acode1, *acode2, *acode3;
	unsigned char *amask0, *amask1, *amask2, *amask3;

	acode0 = &priv->regs->rx_fi_xff;
	acode1 = (unsigned char *)&priv->regs->msg.rst_accept.code[0];
	acode2 = (unsigned char *)&priv->regs->msg.rst_accept.code[1];
	acode3 = (unsigned char *)&priv->regs->msg.rst_accept.code[2];

	amask0 = (unsigned char *)&priv->regs->msg.rst_accept.mask[0];
	amask1 = (unsigned char *)&priv->regs->msg.rst_accept.mask[1];
	amask2 = (unsigned char *)&priv->regs->msg.rst_accept.mask[2];
	amask3 = (unsigned char *)&priv->regs->msg.rst_accept.mask[3];

	/* Set new mask & code */
	WRITE_REG(priv, acode0, acode[0]);
	WRITE_REG(priv, acode1, acode[1]);
	WRITE_REG(priv, acode2, acode[2]);
	WRITE_REG(priv, acode3, acode[3]);

	WRITE_REG(priv, amask0, amask[0]);
	WRITE_REG(priv, amask1, amask[1]);
	WRITE_REG(priv, amask2, amask[2]);
	WRITE_REG(priv, amask3, amask[3]);
}

static void convert_timing_to_btrs(
	struct grlib_canbtrs_timing *t,
	occan_speed_regs *btrs)
{
	btrs->btr0 = (t->rsj << OCCAN_BUSTIM_SJW_BIT) |
	             (t->scaler & OCCAN_BUSTIM_BRP);

	/* Core adds +1 to the register values, so compensate here by decrementing */
	btrs->btr1 = (0<<7) | ((t->ps2-1) << OCCAN_BUSTIM_TSEG2_BIT) | (t->ps1-1);
}

static int occan_set_speedregs(occan_priv *priv, occan_speed_regs *timing)
{
	if ( !timing || !priv || !priv->regs)
		return -1;

	WRITE_REG(priv, &priv->regs->bustim0, timing->btr0);
	WRITE_REG(priv, &priv->regs->bustim1, timing->btr1);

	return 0;
}

static rtems_device_driver occan_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;
	(void) arg;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver occan_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) arg;

	occan_priv *can;
	struct drvmgr_dev *dev;

	DBG("OCCAN: Opening %d\n\r",minor);

	/* get can device */	
	if ( drvmgr_get_dev(&occan_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_UNSATISFIED; /* NODEV */
	}
	can = (occan_priv *)dev->priv;

	/* already opened? */
	rtems_semaphore_obtain(can->devsem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if ( can->open ){
		rtems_semaphore_release(can->devsem);
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */
	}
	can->open = 1;
	rtems_semaphore_release(can->devsem);

	SPIN_INIT(&can->devlock, can->devName);

	/* allocate fifos */
	can->rxfifo = occan_fifo_create(DEFAULT_RX_FIFO_LEN);
	if ( !can->rxfifo ){
		can->open = 0;
		return RTEMS_NO_MEMORY; /* ENOMEM */
	}

	can->txfifo = occan_fifo_create(DEFAULT_TX_FIFO_LEN);
	if ( !can->txfifo ){
		occan_fifo_free(can->rxfifo);
		can->rxfifo= NULL;
		can->open = 0;
		return RTEMS_NO_MEMORY; /* ENOMEM */
	}

	DBG("OCCAN: Opening %d success\n\r",minor);

	can->started = 0;
	can->channel = 0; /* Default to first can link */
	can->txblk = 1; /* Default to Blocking mode */
	can->rxblk = 1; /* Default to Blocking mode */
	can->single_mode = 1; /* single mode acceptance filter */

	/* reset stat counters */
	memset(&can->stats,0,sizeof(occan_stats));

	/* HW must be in reset mode here (close and initializes resets core...)
	 *
	 * 1. set default modes/speeds
	 */
	pelican_open(can);

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver occan_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) arg;

	occan_priv *can;
	struct drvmgr_dev *dev;

	DBG("OCCAN: Closing %d\n\r",minor);

	if ( drvmgr_get_dev(&occan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	can = (occan_priv *)dev->priv;

	/* stop if running */
	if ( can->started )
		pelican_stop(can);

	/* Enter Reset Mode */
	WRITE_REG(can, &can->regs->mode, PELICAN_MOD_RESET);

	/* free fifo memory */
	occan_fifo_free(can->rxfifo);
	occan_fifo_free(can->txfifo);

	can->rxfifo = NULL;
	can->txfifo = NULL;

	can->open = 0;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver occan_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;

	occan_priv *can;
	struct drvmgr_dev *dev;
	rtems_libio_rw_args_t *rw_args=(rtems_libio_rw_args_t *) arg;
	CANMsg *dstmsg, *srcmsg;
	SPIN_IRQFLAGS(oldLevel);
	int left;

	if ( drvmgr_get_dev(&occan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	can = (occan_priv *)dev->priv;

	if ( !can->started ){
		DBG("OCCAN: cannot read from minor %d when not started\n\r",minor);
		return RTEMS_RESOURCE_IN_USE; /* -EBUSY*/
	}

	/* does at least one message fit */
	left = rw_args->count;
	if ( left < sizeof(CANMsg) ){
		DBG("OCCAN: minor %d length of buffer must be at least %d, our is %d\n\r",minor,sizeof(CANMsg),left);
		return  RTEMS_INVALID_NAME; /* -EINVAL */
	}

	/* get pointer to start where to put CAN messages */
	dstmsg = (CANMsg *)rw_args->buffer;
	if ( !dstmsg ){
		DBG("OCCAN: minor %d read: input buffer is NULL\n\r",minor);
		return  RTEMS_INVALID_NAME; /* -EINVAL */
	}

	while (left >= sizeof(CANMsg) ){

		/* turn off interrupts */
		SPIN_LOCK_IRQ(&can->devlock, oldLevel);

		/* A bus off interrupt may have occured after checking can->started */
		if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
			SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);
			DBG("OCCAN: read is cancelled due to a BUS OFF error\n\r");
			rw_args->bytes_moved = rw_args->count-left;
			return RTEMS_IO_ERROR; /* EIO */
		}

		srcmsg = occan_fifo_claim_get(can->rxfifo);
		if ( !srcmsg ){
			/* no more messages in reception fifo.
			 * Wait for incoming packets only if in
			 * blocking mode AND no messages been
			 * read before.
			 */
			if ( !can->rxblk || (left != rw_args->count) ){
				/* turn on interrupts again */
				SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);
				break;
			}

			/* turn on interrupts again */
			SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);

			DBG("OCCAN: Waiting for RX int\n\r");

			/* wait for incoming messages */
			rtems_semaphore_obtain(can->rxsem, RTEMS_WAIT,
				RTEMS_NO_TIMEOUT);

			/* did we get woken up by a BUS OFF error? */
			if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
				DBG("OCCAN: Blocking read got woken up by BUS OFF error\n\r");
				/* At this point it should not matter how many messages we handled */
				rw_args->bytes_moved = rw_args->count-left;
				return RTEMS_IO_ERROR; /* EIO */
			}

			/* no errors detected, it must be a message */
			continue;
		}

		/* got message, copy it to userspace buffer */
		*dstmsg = *srcmsg;

		/* Return borrowed message, RX interrupt can use it again */
		occan_fifo_get(can->rxfifo);

		/* turn on interrupts again */
		SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);

		/* increase pointers */
		left -= sizeof(CANMsg);
		dstmsg++;
	}

	/* save number of read bytes. */
	rw_args->bytes_moved = rw_args->count-left;
	if ( rw_args->bytes_moved == 0 ){
		DBG("OCCAN: minor %d read would block, returning\n\r",minor);
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */
	}
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver occan_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;

	occan_priv *can;
	struct drvmgr_dev *dev;
	rtems_libio_rw_args_t *rw_args=(rtems_libio_rw_args_t *) arg;
	CANMsg *msg,*fifo_msg;
	SPIN_IRQFLAGS(oldLevel);
	int left;

	DBG("OCCAN: Writing %d bytes from 0x%lx (%d)\n\r",rw_args->count,rw_args->buffer,sizeof(CANMsg));

	if ( drvmgr_get_dev(&occan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	can = (occan_priv *)dev->priv;

	if ( !can->started )
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */

	left = rw_args->count;
	if ( (left < sizeof(CANMsg)) || (!rw_args->buffer) ){
		return RTEMS_INVALID_NAME; /* EINVAL */
	}

	msg = (CANMsg *)rw_args->buffer;

	/* limit CAN message length to 8 */
	msg->len = (msg->len > 8) ? 8 : msg->len;

#ifdef DEBUG_VERBOSE
	pelican_regs_print(can->regs);
	occan_stat_print(&can->stats);
#endif

	/* turn off interrupts */
	SPIN_LOCK_IRQ(&can->devlock, oldLevel);

	/* A bus off interrupt may have occured after checking can->started */
	if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
		SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);
		rw_args->bytes_moved = 0;
		return RTEMS_IO_ERROR; /* EIO */
	}

	/* If no messages in software tx fifo, we will
	 * try to send first message by putting it directly
	 * into the HW TX fifo.
	 */
	if ( occan_fifo_empty(can->txfifo) ){
		/*pelican_regs_print(cans[minor+1].regs);*/
		if ( !pelican_send(can,msg) ) {
			/* First message put directly into HW TX fifo
			 * This will turn TX interrupt on.
			 */
			left -= sizeof(CANMsg);
			msg++;

#ifdef OCCAN_TX_IRQ_FLAG_FIXUP
			/* Mark that we have put at least one msg in TX FIFO */
			can->sending = 1;
#endif

			/* bump stat counters */
			can->stats.tx_msgs++;

			DBG("OCCAN: Sending direct via HW\n\r");
		}
	}

	/* Put messages into software fifo */
	while ( left >= sizeof(CANMsg) ){

		/* limit CAN message length to 8 */
		msg->len = (msg->len > 8) ? 8 : msg->len;

		fifo_msg = occan_fifo_put_claim(can->txfifo,0);
		if ( !fifo_msg ){

			DBG("OCCAN: FIFO is full\n\r");
			/* Block only if no messages previously sent
			 * and no in blocking mode
			 */
			if ( !can->txblk  || (left != rw_args->count) )
				break;

			/* turn on interupts again and wait
				INT_ON
				WAIT FOR FREE BUF;
				INT_OFF;
				CHECK_IF_FIFO_EMPTY ==> SEND DIRECT VIA HW;
			*/
			SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);

			DBG("OCCAN: Waiting for tx int\n\r");

			rtems_semaphore_obtain(can->txsem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

			/* did we get woken up by a BUS OFF error? */
			if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
				DBG("OCCAN: Blocking write got woken up by BUS OFF error or RESET event\n\r");
				/* At this point it should not matter how many messages we handled */
				rw_args->bytes_moved = rw_args->count-left;
				return RTEMS_IO_ERROR; /* EIO */
			}

			SPIN_LOCK_IRQ(&can->devlock, oldLevel);

			if ( occan_fifo_empty(can->txfifo) ){
				if ( !pelican_send(can,msg) ) {
					/* First message put directly into HW TX fifo
					 * This will turn TX interrupt on.
					 */
					left -= sizeof(CANMsg);
					msg++;

#ifdef OCCAN_TX_IRQ_FLAG_FIXUP
					/* Mark that we have put at least one msg in TX FIFO */
					can->sending = 1;
#endif

					/* bump stat counters */
					can->stats.tx_msgs++;

					DBG("OCCAN: Sending direct2 via HW\n\r");
				}
			}
			continue;
		}

		/* copy message into fifo area */
		*fifo_msg = *msg;

		/* tell interrupt handler about the message */
		occan_fifo_put(can->txfifo);

		DBG("OCCAN: Put info fifo SW\n\r");

		/* Prepare insert of next message */
		msg++;
		left-=sizeof(CANMsg);
	}

	SPIN_UNLOCK_IRQ(&can->devlock, oldLevel);

	rw_args->bytes_moved = rw_args->count-left;
	DBG("OCCAN: Sent %d\n\r",rw_args->bytes_moved);

	if ( left == rw_args->count )
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver occan_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;

	int ret;
	struct grlib_canbtrs_timing timing;
	occan_priv *can;
	struct drvmgr_dev *dev;
	unsigned int speed;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
	struct occan_afilter *afilter;
	occan_stats *dststats;
	unsigned int rxcnt,txcnt;

	DBG("OCCAN: IOCTL %d\n\r",ioarg->command);

	if ( drvmgr_get_dev(&occan_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	can = (occan_priv *)dev->priv;

	ioarg->ioctl_return = 0;
	switch(ioarg->command){
		case OCCAN_IOC_SET_SPEED:

			/* cannot change speed during run mode */
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			/* get speed rate from argument */
			speed = (unsigned int)ioarg->buffer;
			/* Calculate default timing register values */
			ret = grlib_canbtrs_calc_timing(
				speed, can->sys_freq_hz,
				OCCAN_SAMPLING_POINT, &occan_btrs_ranges,
				(struct grlib_canbtrs_timing *)&timing);
			if ( ret )
				return  RTEMS_INVALID_NAME; /* EINVAL */

			/* set the speed regs of the CAN core */
			/* occan_set_speedregs(can,timing); */

			/* save timing/speed */
			can->speed = speed;
			convert_timing_to_btrs(&timing, &can->timing);
			break;

		case OCCAN_IOC_SET_BTRS:
			/* Set BTR registers manually
			 * Read OCCAN Manual.
			 */
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			can->speed = 0; /* custom */
			can->timing.btr1 = (unsigned int)ioarg->buffer & 0xff;
			can->timing.btr0 = ((unsigned int)ioarg->buffer>>8) & 0xff;
/*
			can->timing.sjw = (btr0 >> OCCAN_BUSTIM_SJW_BIT) & 0x3;
			can->timing.brp = btr0 & OCCAN_BUSTIM_BRP;
			can->timing.tseg1 = btr1 & 0xf;
			can->timing.tseg2 = (btr1 >> OCCAN_BUSTIM_TSEG2_BIT) & 0x7;
			can->timing.sam = (btr1 >> 7) & 0x1;
			*/
			break;

		case OCCAN_IOC_SPEED_AUTO:
			return RTEMS_NOT_IMPLEMENTED;

		case OCCAN_IOC_SET_BUFLEN:
			/* set rx & tx fifo buffer length */
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			rxcnt = (unsigned int)ioarg->buffer & 0x0000ffff;
			txcnt = (unsigned int)ioarg->buffer >> 16;

			occan_fifo_free(can->rxfifo);
			occan_fifo_free(can->txfifo);

			/* allocate new buffers */
			can->rxfifo = occan_fifo_create(rxcnt);
			can->txfifo = occan_fifo_create(txcnt);

			if ( !can->rxfifo || !can->txfifo )
				return RTEMS_NO_MEMORY; /* ENOMEM */
			break;

		case OCCAN_IOC_GET_CONF:
			return RTEMS_NOT_IMPLEMENTED;
			break;

		case OCCAN_IOC_GET_STATS:
			dststats = (occan_stats *)ioarg->buffer;
			if ( !dststats )
				return  RTEMS_INVALID_NAME; /* EINVAL */

			/* copy data stats into userspace buffer */
			if ( can->rxfifo )
				can->stats.rx_sw_dovr = can->rxfifo->ovcnt;
			*dststats = can->stats;
			break;

		case OCCAN_IOC_GET_STATUS:
			/* return the status of the */
			if ( !ioarg->buffer )
				return RTEMS_INVALID_NAME;

			*(unsigned int *)ioarg->buffer = can->status;
			break;

		/* Set physical link */
		case OCCAN_IOC_SET_LINK:
#ifdef REDUNDANT_CHANNELS
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			/* switch HW channel */
			can->channel = (unsigned int)ioargs->buffer;
#else
			return RTEMS_NOT_IMPLEMENTED;
#endif
			break;

		case OCCAN_IOC_SET_FILTER:
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			afilter = (struct occan_afilter *)ioarg->buffer;

			if ( !afilter )
				return RTEMS_INVALID_NAME; /* EINVAL */

			/* copy acceptance filter */
			can->acode[0] = afilter->code[0];
			can->acode[1] = afilter->code[1];
			can->acode[2] = afilter->code[2];
			can->acode[3] = afilter->code[3];

			can->amask[0] = afilter->mask[0];
			can->amask[1] = afilter->mask[1];
			can->amask[2] = afilter->mask[2];
			can->amask[3] = afilter->mask[3];

			can->single_mode = ( afilter->single_mode ) ? 1 : 0;

			/* Acceptance filter is written to hardware
			 * when starting.
			 */
			/* pelican_set_accept(can,can->acode,can->amask);*/
			break;

		case OCCAN_IOC_SET_BLK_MODE:
			can->rxblk = (unsigned int)ioarg->buffer & OCCAN_BLK_MODE_RX;
			can->txblk = ((unsigned int)ioarg->buffer & OCCAN_BLK_MODE_TX) >> 1;
			break;

		case OCCAN_IOC_START:
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */
			if ( pelican_start(can) )
				return RTEMS_NO_MEMORY; /* failed because of no memory, can happen if SET_BUFLEN failed */
			/* can->started = 1; -- Is set in pelican_start due to interrupt may occur before we 
			 * get here.
			 */
			break;

		case OCCAN_IOC_STOP:
			if ( !can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */
			pelican_stop(can);
			can->started = 0;
			break;

		default:
			return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}

void occan_interrupt(void *arg)
{
	occan_priv *can = arg;
	unsigned char iflags;
	pelican_regs *regs = can->regs;
	CANMsg *msg;
	int signal_rx=0, signal_tx=0;
	unsigned char tmp, errcode, arbcode;
	int tx_error_cnt,rx_error_cnt;
	SPIN_ISR_IRQFLAGS(irqflags);

	if ( !can->started )
		return; /* Spurious Interrupt, do nothing */

	SPIN_LOCK(&can->devlock, irqflags);
	while (1) {

		iflags = READ_REG(can, &can->regs->intflags);

#ifdef OCCAN_TX_IRQ_FLAG_FIXUP
		/* TX IRQ may be cleared when reading regs->intflags due
		 * to a bug in some chips. Instead of looking at the TX_IRQ_FLAG
		 * the TX-fifo emoty register is looked at when something has
		 * been scheduled for transmission.
		 */
		if ((iflags & PELICAN_IF_TX) == 0) {
			if (can->sending && pelican_tx_ready(can)) {
				can->sending = 0;
				iflags |= PELICAN_IF_TX;
			}
		}
#endif

		if (iflags == 0)
			break;
		/* still interrupts to handle */

		can->stats.ints++;

		if ( iflags & PELICAN_IF_RX ){
			/* the rx fifo is not empty
			 * put 1 message into rxfifo for later use
			 */

			/* get empty (or make room) message */
			msg = occan_fifo_put_claim(can->rxfifo,1);
			tmp = READ_REG(can, &regs->rx_fi_xff);
			msg->extended = tmp >> 7;
			msg->rtr = (tmp >> 6) & 1;
			msg->len = tmp = tmp & 0x0f;

			if ( msg->extended ){
				/* extended message */
				msg->id =  READ_REG(can, &regs->msg.rx_eff.id[0])<<(5+8+8) |
				           READ_REG(can, &regs->msg.rx_eff.id[1])<<(5+8) |
				           READ_REG(can, &regs->msg.rx_eff.id[2])<<5 |
				           READ_REG(can, &regs->msg.rx_eff.id[3])>>3;

				while(tmp--){
					msg->data[tmp] = READ_REG(can, &regs->msg.rx_eff.data[tmp]);
				}
				/*
				msg->data[0] = READ_REG(can, &regs->msg.rx_eff.data[0]);
				msg->data[1] = READ_REG(can, &regs->msg.rx_eff.data[1]);
				msg->data[2] = READ_REG(can, &regs->msg.rx_eff.data[2]);
				msg->data[3] = READ_REG(can, &regs->msg.rx_eff.data[3]);
				msg->data[4] = READ_REG(can, &regs->msg.rx_eff.data[4]);
				msg->data[5] = READ_REG(can, &regs->msg.rx_eff.data[5]);
				msg->data[6] = READ_REG(can, &regs->msg.rx_eff.data[6]);
				msg->data[7] = READ_REG(can, &regs->msg.rx_eff.data[7]);
				*/
			}else{
				/* standard message */
				msg->id = READ_REG(can, &regs->msg.rx_sff.id[0])<<3 |
				          READ_REG(can, &regs->msg.rx_sff.id[1])>>5;

				while(tmp--){
					msg->data[tmp] = READ_REG(can, &regs->msg.rx_sff.data[tmp]);
				}
				/*
				msg->data[0] = READ_REG(can, &regs->msg.rx_sff.data[0]);
				msg->data[1] = READ_REG(can, &regs->msg.rx_sff.data[1]);
				msg->data[2] = READ_REG(can, &regs->msg.rx_sff.data[2]);
				msg->data[3] = READ_REG(can, &regs->msg.rx_sff.data[3]);
				msg->data[4] = READ_REG(can, &regs->msg.rx_sff.data[4]);
				msg->data[5] = READ_REG(can, &regs->msg.rx_sff.data[5]);
				msg->data[6] = READ_REG(can, &regs->msg.rx_sff.data[6]);
				msg->data[7] = READ_REG(can, &regs->msg.rx_sff.data[7]);
				*/
			}

			/* Re-Enable RX buffer for a new message */
			WRITE_REG(can, &regs->cmd, PELICAN_CMD_RELRXBUF);

			/* make message available to the user */
			occan_fifo_put(can->rxfifo);

			/* bump stat counters */
			can->stats.rx_msgs++;

			/* signal the semaphore only once */
			signal_rx = 1;
		}

		if ( iflags & PELICAN_IF_TX ) {

			/* there is room in tx fifo of HW */

			if ( !occan_fifo_empty(can->txfifo) ){
				/* send 1 more messages */
				msg = occan_fifo_claim_get(can->txfifo);

				if ( pelican_send(can,msg) ){
					/* ERROR! We got an TX interrupt telling us
					 * tx fifo is empty, yet it is not.
					 *
					 * Complain about this max 10 times
					 */
					if ( can->stats.tx_buf_error < 10 ){
						printk("OCCAN: got TX interrupt but TX fifo in not empty (%d)\n\r",can->stats.tx_buf_error);
					}
					can->status |= OCCAN_STATUS_QUEUE_ERROR;
					can->stats.tx_buf_error++;
				}
#ifdef OCCAN_TX_IRQ_FLAG_FIXUP
				can->sending = 1;
#endif

				/* free software-fifo space taken by sent message */
				occan_fifo_get(can->txfifo);

				/* bump stat counters */
				can->stats.tx_msgs++;

				/* wake any sleeping thread waiting for "fifo not full" */
				signal_tx = 1;
			}
		}

		if ( iflags & PELICAN_IF_ERRW ){
			tx_error_cnt = READ_REG(can, &regs->tx_err_cnt);
			rx_error_cnt = READ_REG(can, &regs->rx_err_cnt);

			/* 1. if bus off tx error counter = 127 */
			if ( (tx_error_cnt > 96) || (rx_error_cnt > 96) ){
				/* in Error Active Warning area or BUS OFF */
				can->status |= OCCAN_STATUS_WARN;

				/* check reset bit for reset mode */
				if ( READ_REG(can, &regs->mode) & PELICAN_MOD_RESET ){
					/* in reset mode ==> bus off */
					can->status |= OCCAN_STATUS_ERR_BUSOFF | OCCAN_STATUS_RESET;

					/***** pelican_stop(can) ******
					 * turn off interrupts
					 * enter reset mode (HW already done that for us)
					 */
					WRITE_REG(can, &regs->inten,0);

					/* Indicate that we are not started any more.
					 * This will make write/read return with EBUSY
					 * on read/write attempts.
					 *
					 * User must issue a ioctl(START) to get going again.
					 */
					can->started = 0;

					/* signal any waiting read/write threads, so that they
					 * can handle the bus error.
					 */
					signal_rx = 1;
					signal_tx = 1;

					/* ingnore any old pending interrupt */
					break;
				}

			}else{
				/* not in Upper Error Active area any more */
				can->status &= ~(OCCAN_STATUS_WARN);
			}
			can->stats.err_warn++;
		}

		if ( iflags & PELICAN_IF_DOVR){
			can->status |= OCCAN_STATUS_OVERRUN;
			can->stats.err_dovr++;
			DBG("OCCAN_INT: DOVR\n\r");
		}

		if ( iflags & PELICAN_IF_ERRP){
			/* Let the error counters decide what kind of
			 * interrupt it was. In/Out of EPassive area.
			 */
			tx_error_cnt = READ_REG(can, &regs->tx_err_cnt);
			rx_error_cnt = READ_REG(can, &regs->rx_err_cnt);

			if ( (tx_error_cnt > 127) || (rx_error_cnt > 127) ){
				can->status |= OCCAN_STATUS_ERR_PASSIVE;
			}else{
				can->status &= ~(OCCAN_STATUS_ERR_PASSIVE);
			}

			/* increase Error Passive In/out interrupt counter */
			can->stats.err_errp++;
		}

		if ( iflags & PELICAN_IF_ARB){
			arbcode = READ_REG(can, &regs->arbcode);
			can->stats.err_arb_bitnum[arbcode & PELICAN_ARB_BITS]++;
			can->stats.err_arb++;
			DBG("OCCAN_INT: ARB (0x%x)\n\r",arbcode & PELICAN_ARB_BITS);
		}

		if ( iflags & PELICAN_IF_BUS){
			/* Some kind of BUS error, only used for
			 * statistics. Error Register is decoded
			 * and put into can->stats.
			 */
			errcode = READ_REG(can, &regs->errcode);
			switch( errcode & PELICAN_ECC_CODE ){
				case PELICAN_ECC_CODE_BIT:
					can->stats.err_bus_bit++;
					break;
				case PELICAN_ECC_CODE_FORM:
					can->stats.err_bus_form++;
					break;
				case PELICAN_ECC_CODE_STUFF:
					can->stats.err_bus_stuff++;
					break;
				case PELICAN_ECC_CODE_OTHER:
					can->stats.err_bus_other++;
					break;
			}

			/* Get Direction (TX/RX) */
			if ( errcode & PELICAN_ECC_DIR ){
				can->stats.err_bus_rx++;
			}else{
				can->stats.err_bus_tx++;
			}

			/* Get Segment in frame that went wrong */
			can->stats.err_bus_segs[errcode & PELICAN_ECC_SEG]++;

			/* total number of bus errors */
			can->stats.err_bus++;
		}
	}
	SPIN_UNLOCK(&can->devlock, irqflags);

	/* signal Binary semaphore, messages available! */
	if ( signal_rx ){
		rtems_semaphore_release(can->rxsem);
	}

	if ( signal_tx ){
		rtems_semaphore_release(can->txsem);
	}
}

/*******************************************************************************
 * FIFO IMPLEMENTATION
 */

static occan_fifo *occan_fifo_create(int cnt)
{
	occan_fifo *fifo;
	fifo = grlib_calloc(1, sizeof(*fifo)+cnt*sizeof(CANMsg));
	if ( fifo ){
		fifo->cnt = cnt;
		fifo->base = &fifo->fifoarea[0];
		fifo->tail = fifo->head = fifo->base;
	}
	return fifo;
}

static void occan_fifo_free(occan_fifo *fifo)
{
	if ( fifo )
		free(fifo);
}

static int occan_fifo_full(occan_fifo *fifo)
{
	return fifo->full;
}

static int occan_fifo_empty(occan_fifo *fifo)
{
	return (!fifo->full) && (fifo->head == fifo->tail);
}

/* Stage 1 - get buffer to fill (never fails if force!=0) */
static CANMsg *occan_fifo_put_claim(occan_fifo *fifo, int force)
{
	if ( !fifo )
		return NULL;

	if ( occan_fifo_full(fifo) ){

		if ( !force )
			return NULL;

		/* all buffers already used ==> overwrite the oldest */
		fifo->ovcnt++;
		occan_fifo_get(fifo);
	}

	return fifo->head;
}

/* Stage 2 - increment indexes */
static void occan_fifo_put(occan_fifo *fifo)
{
	if ( occan_fifo_full(fifo) )
		return;

	/* wrap around */
	fifo->head = (fifo->head >= &fifo->base[fifo->cnt-1])? fifo->base : fifo->head+1;

	if ( fifo->head == fifo->tail )
		fifo->full = 1;
}

static CANMsg *occan_fifo_claim_get(occan_fifo *fifo)
{
	if ( occan_fifo_empty(fifo) )
		return NULL;

	/* return oldest message */
	return fifo->tail;
}


static void occan_fifo_get(occan_fifo *fifo)
{
	if ( !fifo )
		return;

	if ( occan_fifo_empty(fifo) )
		return;

	/* increment indexes */
	fifo->tail = (fifo->tail >= &fifo->base[fifo->cnt-1]) ?
	             fifo->base : fifo->tail+1;
	fifo->full = 0;
}

static void occan_fifo_clr(occan_fifo *fifo)
{
	fifo->full  = 0;
	fifo->ovcnt = 0;
	fifo->head  = fifo->tail = fifo->base;
}

/******************************************************************************/
