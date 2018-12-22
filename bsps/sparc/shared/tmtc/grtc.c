/* GRTC Telecommand decoder driver
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

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>
#include <grlib/grtc.h>

#include <grlib/grlib_impl.h>

/*
#define DEBUG
#define DEBUGFUNCS
*/

#include <grlib/debug_defs.h>

#ifdef DEBUG_ERROR
#define DEBUG_ERR_LOG(device,error) grtc_log_error(device,error)
#else
#define DEBUG_ERR_LOG(device,error)
#endif

/* GRTC register map */
struct grtc_regs {
	volatile unsigned int	grst;	/* Global Reset Register (GRR 0x00) */
	volatile unsigned int	gctrl;	/* Global Control Register (GCR 0x04) */
	int unused0;
	volatile unsigned int	sir;	/* Spacecraft Identifier Register (SIR 0x0c) */	
	volatile unsigned int	far;	/* Frame Acceptance Report Register (FAR 0x10) */

	volatile unsigned int	clcw1;	/* CLCW Register 1 (CLCWR1 0x14) */
	volatile unsigned int	clcw2;	/* CLCW Register 2 (CLCWR2 0x18) */
	volatile unsigned int	phir;	/* Physical Interface Register (PHIR 0x1c) */
	volatile unsigned int	cor;	/* Control Register (COR 0x20) */

	volatile unsigned int	str;	/* Status Register (STR 0x24) */
	volatile unsigned int	asr;	/* Address Space Register (ASR 0x28) */
	volatile unsigned int	rp;	/* Receive Read Pointer Register (RRP 0x2c) */
	volatile unsigned int	wp;	/* Receive Write Pointer Register (RWP 0x30) */

	int unused1[(0x60-0x34)/4];

	volatile unsigned int	pimsr;	/* Pending Interrupt Masked Status Register (PIMSR 0x60) */
	volatile unsigned int	pimr;	/* Pending Interrupt Masked Register (PIMR 0x64) */
	volatile unsigned int	pisr;	/* Pending Interrupt Status Register (PISR 0x68) */
	volatile unsigned int	pir;	/* Pending Interrupt Register (PIR 0x6c) */
	volatile unsigned int	imr;	/* Interrupt Mask Register (IMR 0x70) */
	volatile unsigned int	picr;	/* Pending Interrupt Clear Register (PICR 0x74) */
};

/* Security Byte */
#define GRTC_SEB		0x55000000

/* Global Reset Register (GRR 0x00) */
#define GRTC_GRR_SRST		0x1
#define GRTC_GRR_SRST_BIT	0

/* Global Control Register (GCR 0x04) */
#define GRTC_GCR_PSR_BIT	10
#define GRTC_GCR_NRZM_BIT	11
#define GRTC_GCR_PSS_BIT	12

#define GRTC_GCR_PSR		(1<<GRTC_GCR_PSR_BIT)
#define GRTC_GCR_NRZM		(1<<GRTC_GCR_NRZM_BIT)
#define GRTC_GCR_PSS		(1<<GRTC_GCR_PSS_BIT)

/* Spacecraft Identifier Register (SIR 0x0c) */	


/* Frame Acceptance Report Register (FAR 0x10) */
#define GRTC_FAR_SCI_BIT	10
#define GRTC_FAR_CSEC_BIT	11
#define GRTC_FAR_CAC_BIT	12
#define GRTC_FAR_SSD_BIT	13

#define GRTC_FAR_SCI		(0x7<<GRTC_FAR_SCI_BIT)
#define GRTC_FAR_CSEC		(0x7<<GRTC_FAR_CSEC_BIT)
#define GRTC_FAR_CAC		(0x3f<<GRTC_FAR_CAC_BIT)
#define GRTC_FAR_SSD		(1<<GRTC_FAR_SSD_BIT)

/* CLCW Register 1 (CLCWR1 0x14) */
/* CLCW Register 2 (CLCWR2 0x18) */
#define GRTC_CLCW_RVAL_BIT	0
#define GRTC_CLCW_RTYPE_BIT	8
#define GRTC_CLCW_FBCO_BIT	9
#define GRTC_CLCW_RTMI_BIT	11
#define GRTC_CLCW_WAIT_BIT	12
#define GRTC_CLCW_LOUT_BIT	13
#define GRTC_CLCW_NBLO_BIT	14
#define GRTC_CLCW_NRFA_BIT	15
#define GRTC_CLCW_VCI_BIT	18
#define GRTC_CLCW_CIE_BIT	24
#define GRTC_CLCW_STAF_BIT	26
#define GRTC_CLCW_VNUM_BIT	29
#define GRTC_CLCW_CWTY_BIT	31

#define GRTC_CLCW_RVAL		(0xff<<GRTC_CLCW_RVAL_BIT)
#define GRTC_CLCW_RTYPE		(1<<GRTC_CLCW_RTYPE_BIT)
#define GRTC_CLCW_FBCO		(0x3<<GRTC_CLCW_FBCO_BIT)
#define GRTC_CLCW_RTMI		(0x3<<GRTC_CLCW_RTMI_BIT)
#define GRTC_CLCW_WAIT		(1<<GRTC_CLCW_WAIT_BIT)
#define GRTC_CLCW_LOUT		(1<<GRTC_CLCW_LOUT_BIT)
#define GRTC_CLCW_NBLO		(1<<GRTC_CLCW_NBLO_BIT)
#define GRTC_CLCW_NRFA		(1<<GRTC_CLCW_NRFA_BIT)
#define GRTC_CLCW_VCI		(0x3f<<GRTC_CLCW_VCI_BIT)
#define GRTC_CLCW_CIE		(0x3<<GRTC_CLCW_CIE_BIT)
#define GRTC_CLCW_STAF		(0x3<<GRTC_CLCW_STAF_BIT)
#define GRTC_CLCW_VNUM		(0x3<<GRTC_CLCW_VNUM_BIT)
#define GRTC_CLCW_CWTY		(1<<GRTC_CLCW_CWTY_BIT)

/* Physical Interface Register (PIR 0x1c) */
#define GRTC_PIR_BLO_BIT	0
#define GRTC_PIR_RFA_BIT	8

#define GRTC_PIR_BLO		(0xff<<GRTC_PIR_BLO_BIT)
#define GRTC_PIR_RFA		(0xff<<GRTC_PIR_RFA_BIT)

/* Control Register (COR 0x20) */
#define GRTC_COR_RE_BIT		0
#define GRTC_COR_CRST_BIT	9

#define GRTC_COR_RE		(1<<GRTC_COR_RE_BIT)
#define GRTC_COR_CRST		(1<<GRTC_COR_CRST_BIT)

/* Status Register (STR 0x24) */
#define GRTC_STR_CR_BIT		0
#define GRTC_STR_OV_BIT		4
#define GRTC_STR_RFF_BIT	7
#define GRTC_STR_RBF_BIT	10

#define GRTC_STR_CR		(1<<GRTC_STR_CR_BIT)
#define GRTC_STR_OV		(1<<GRTC_STR_OV_BIT)
#define GRTC_STR_RFF		(1<<GRTC_STR_RFF_BIT)
#define GRTC_STR_RBF		(1<<GRTC_STR_RBF_BIT)

/* Address Space Register (ASR 0x28) */
#define GRTC_ASR_RXLEN_BIT	0
#define GRTC_ASR_BUFST_BIT	10

#define GRTC_ASR_RXLEN		(0xff<<GRTC_ASR_RXLEN_BIT)
#define GRTC_ASR_BUFST		(0x3fffff<<GRTC_ASR_BUFST_BIT)

/* Receive Read Pointer Register (RRP 0x2c) */
#define GRTC_RRP_PTR_BIT	0

#define GRTC_RRP_PTR		(0xffffff<<GRTC_RRP_PTR_BIT)

/* Receive Write Pointer Register (RWP 0x30) */
#define GRTC_RWP_PTR_BIT	0

#define GRTC_RWP_PTR		(0xffffff<<GRTC_RWP_PTR_BIT)

/* Pending Interrupt Masked Status Register (PIMSR 0x60) */
/* Pending Interrupt Masked Register (PIMR 0x64) */
/* Pending Interrupt Status Register (PISR 0x68) */
/* Pending Interrupt Register (PIR 0x6c) */
/* Interrupt Mask Register (IMR 0x70) */
/* Pending Interrupt Clear Register (PICR 0x74) */
#define GRTC_INT_RFA_BIT	0
#define GRTC_INT_BLO_BIT	1
#define GRTC_INT_FAR_BIT	2
#define GRTC_INT_CR_BIT		3
#define GRTC_INT_RBF_BIT	4
#define GRTC_INT_OV_BIT		5
#define GRTC_INT_CS_BIT		6

#define GRTC_INT_RFA		(1<<GRTC_INT_RFA_BIT)
#define GRTC_INT_BLO		(1<<GRTC_INT_BLO_BIT)
#define GRTC_INT_FAR		(1<<GRTC_INT_FAR_BIT)
#define GRTC_INT_CR		(1<<GRTC_INT_CR_BIT)
#define GRTC_INT_OV		(1<<GRTC_INT_OV_BIT)
#define GRTC_INT_CS		(1<<GRTC_INT_CS_BIT)

#define GRTC_INT_ALL		(GRTC_INT_RFA|GRTC_INT_BLO|GRTC_INT_FAR|GRTC_INT_CR|GRTC_INT_OV|GRTC_INT_CS)

#define READ_REG(address)	(*(volatile unsigned int *)address)

/* Driver functions */
static rtems_device_driver grtc_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg);
static rtems_device_driver grtc_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtc_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtc_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtc_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtc_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define GRTC_DRIVER_TABLE_ENTRY { grtc_initialize, grtc_open, grtc_close, grtc_read, grtc_write, grtc_ioctl }

static rtems_driver_address_table grtc_driver = GRTC_DRIVER_TABLE_ENTRY;

enum {
	FRM_STATE_NONE = 0,		/* not started */
	FRM_STATE_HDR = 1,		/* Reading Header (Frame length isn't known) */
	FRM_STATE_ALLOC = 2,		/* Allocate Frame to hold data */
	FRM_STATE_PAYLOAD = 3,		/* Reading Payload (Frame length is known) */
	FRM_STATE_FILLER = 4,		/* Check filler */
	FRM_STATE_DROP = 5		/* error, drop data until end marker */
};

/* Frame pool, all frames in pool have the same buffer length (frame mode only) */
struct grtc_frame_pool {
	unsigned int		frame_len;	/* Maximal length of frame (payload+hdr+crc..) */
	unsigned int		frame_cnt;	/* Current number of frames in pool (in frms) */
	struct grtc_frame	*frms;		/* Chain of frames in pool (this is the pool) */
};

struct grtc_priv {
	struct drvmgr_dev		*dev;		/* Driver manager device */
	char			devName[32];	/* Device Name */
	struct grtc_regs	*regs;		/* TC Hardware Register MAP */
	int			irq;		/* IRQ number of TC core */
	SPIN_DECLARE(devlock);			/* spin-lock of registers */

	int			major;		/* Driver major */
	int			minor;		/* Device Minor */

	int			open;		/* Device has been opened by user */
	int			running;	/* TC receiver running */
	int			mode;		/* RAW or FRAME mode */
	int			overrun_condition;	/* Overrun condition */
	int			blocking;	/* Blocking/polling mode */
	rtems_interval		timeout;	/* Timeout in blocking mode */
	int			wait_for_nbytes;/* Number of bytes to wait for in blocking mode */

	struct grtc_ioc_config	config;

/* RAW MODE ONLY */
	/* Buffer allocation (user provided or driver allocated using malloc) */
	void			*buf;
	void			*buf_remote;
	void			*_buf;
	int			buf_custom;	/* 0=no custom buffer, 1=custom buffer (don't free it...) */
	unsigned int		len;

/* FRAME MODE ONLY */
	/* Frame management when user provides buffers. */
	int			pool_cnt;	/* Number of Pools */
	struct grtc_frame_pool	*pools;		/* Array of pools */

	struct grtc_list	ready;		/* Ready queue (received frames) */

	/* Frame read data (Frame mode only) */
	int			frame_state;
	int			filler;
	unsigned int		hdr[2];		/* 5 byte header */
	struct grtc_frame	*frm;		/* Frame currently beeing copied */
	int			frmlen;

	struct grtc_ioc_stats	stats;		/* Statistics */

	rtems_id sem_rx;

#ifdef DEBUG_ERROR	
	/* Buffer read/write state */
	unsigned int rp;
	unsigned int 	wp;

	/* Debugging */
	int last_error[128];
	int last_error_cnt;
#endif
};

/* Prototypes */
static void grtc_hw_reset(struct grtc_priv *priv);
static void grtc_interrupt(void *arg);

/* Common Global Variables */
static rtems_id grtc_dev_sem;
static int grtc_driver_io_registered = 0;
static rtems_device_major_number grtc_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
static int grtc_register_io(rtems_device_major_number *m);
static int grtc_device_init(struct grtc_priv *pDev);

static int grtc_init2(struct drvmgr_dev *dev);
static int grtc_init3(struct drvmgr_dev *dev);

static struct drvmgr_drv_ops grtc_ops = 
{
	{NULL, grtc_init2, grtc_init3, NULL},
	NULL,
	NULL,
};

static struct amba_dev_id grtc_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRTC},
	{0, 0}		/* Mark end of table */
};

static struct amba_drv_info grtc_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRTC_ID,	/* Driver ID */
		"GRTC_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grtc_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct grtc_priv),
	},
	&grtc_ids[0]
};

void grtc_register_drv (void)
{
	DBG("Registering GRTC driver\n");
	drvmgr_drv_register(&grtc_drv_info.general);
}

static int grtc_init2(struct drvmgr_dev *dev)
{
	struct grtc_priv *priv;

	DBG("GRTC[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv;
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

static int grtc_init3(struct drvmgr_dev *dev)
{
	struct grtc_priv *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( grtc_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( grtc_register_io(&grtc_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		grtc_driver_io_registered = 1;
	}
	
	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */
	if ( grtc_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/grtc%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgrtc%d", prefix, dev->minor_bus);
	}

	SPIN_INIT(&priv->devlock, priv->devName);

	/* Register Device */
	status = rtems_io_register_name(priv->devName, grtc_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

static int grtc_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &grtc_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("GRTC driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("GRTC rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("GRTC rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("GRTC rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("GRTC rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

static int grtc_device_init(struct grtc_priv *pDev)
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
	pDev->regs = (struct grtc_regs *)pnpinfo->ahb_slv->start[0];
	pDev->minor = pDev->dev->minor_drv;
	pDev->open = 0;
	pDev->running = 0;

	/* Create Binary RX Semaphore with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'R', 'C', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->sem_rx) != RTEMS_SUCCESSFUL ) {
		return -1;
	}

	/* Reset Hardware before attaching IRQ handler */
	grtc_hw_reset(pDev);

	return 0;
}

static void grtc_hw_reset(struct grtc_priv *priv)
{
	/* Reset Core */
	priv->regs->grst = GRTC_SEB | GRTC_GRR_SRST;
}

static void grtc_hw_get_defaults(struct grtc_priv *pDev, struct grtc_ioc_config *config)
{
	unsigned int gcr = READ_REG(&pDev->regs->gctrl);

	config->psr_enable	= (gcr & GRTC_GCR_PSR)	? 1:0;
	config->nrzm_enable	= (gcr & GRTC_GCR_NRZM)	? 1:0;
	config->pss_enable	= (gcr & GRTC_GCR_PSS)	? 1:0;
	
	config->crc_calc	= 0;
}

/* bufsize is given in bytes */
static int __inline__ grtc_hw_data_avail_upper(unsigned int rrp, unsigned rwp, unsigned int bufsize)
{
	if ( rrp == rwp )
		return 0;

	if ( rwp > rrp ) {
		return rwp-rrp;
	}

	return (bufsize-rrp);
}

/* bufsize is given in bytes */
static int __inline__ grtc_hw_data_avail_lower(unsigned int rrp, unsigned rwp, unsigned int bufsize)
{
	if ( rrp == rwp )
		return 0;

	if ( rwp > rrp ) {
		return 0;
	}

	return rwp;
}

/* bufsize is given in bytes */
static int __inline__ grtc_hw_data_avail(unsigned int rrp, unsigned rwp, unsigned int bufsize)
{
	if ( rrp == rwp )
		return 0;

	if ( rwp > rrp ) {
		return rwp-rrp;
	}

	return rwp+(bufsize-rrp);
}

/* Reads as much as possible but not more than 'max' bytes from the TC receive buffer.
 * Number of bytes put into 'buf' is returned.
 */
static int grtc_hw_read_try(struct grtc_priv *pDev, char *buf, int max)
{
	struct grtc_regs *regs = pDev->regs;
	unsigned int rp, wp, asr, bufmax, rrp, rwp;
	unsigned int upper, lower;
	unsigned int count, cnt, left;

	FUNCDBG();

	if ( max < 1 )
		return 0;
	
	rp = READ_REG(&regs->rp);
	asr = READ_REG(&regs->asr);
	bufmax = (asr & GRTC_ASR_RXLEN) >> GRTC_ASR_RXLEN_BIT;
	bufmax = (bufmax+1) << 10; /* Convert from 1kbyte blocks into bytes */
	wp = READ_REG(&regs->wp);
	
	/* Relative rp and wp */
	rrp = rp - (asr & GRTC_ASR_BUFST);
	rwp = wp - (asr & GRTC_ASR_BUFST);
	
	lower = grtc_hw_data_avail_lower(rrp,rwp,bufmax);
	upper = grtc_hw_data_avail_upper(rrp,rwp,bufmax);
	
	DBG("grtc_hw_read_try: AVAIL: Lower: %d, Upper: %d\n",lower,upper);
	DBG("grtc_hw_read_try: rp: 0x%x, rrp: 0x%x, wp: 0x%x, rwp: 0x%x, bufmax: %d\n, start: 0x%x\n",
		rp,rrp,wp,rwp,bufmax,pDev->buffer);
	
	if ( (upper+lower) == 0 )
		return 0;
	
	/* Count bytes will be read */
	count = (upper+lower) > max ? max : (upper+lower);
	left = count;
	
	/* Read from upper part of data buffer */
	if ( upper > 0 ){
		if ( left < upper ){
			cnt = left;
		}else{
			cnt = upper;	/* Read all upper data available */
		}
		DBG("grtc_hw_read_try: COPYING %d from upper\n",cnt);
		/* Convert from Remote address (RP) into CPU Local address */
		memcpy(buf, (void *)((rp - (unsigned int)pDev->buf_remote) + (unsigned int)pDev->buf), cnt);
		buf += cnt;
		left -= cnt;
	}

	/* Read from lower part of data buffer */
	if ( left > 0 ){
		if ( left < lower ){
			cnt = left;
		}else{
			cnt = lower;	/* Read all lower data available */
		}
		DBG("grtc_hw_read_try: COPYING %d from lower\n",cnt);
		memcpy(buf, (void *)pDev->buf, cnt);
		buf += cnt;
		left -= cnt;
	}
	
	/* Update hardware RP pointer to tell hardware about new space available */
	if ( (rp+count) >= ((asr&GRTC_ASR_BUFST)+bufmax) ){
		regs->rp = (rp+count-bufmax);
	} else {
		regs->rp = rp+count;
	}

	return count;
}

/* Reads as much as possible but not more than 'max' bytes from the TC receive buffer.
 * Number of bytes put into 'buf' is returned.
 */
static int grtc_data_avail(struct grtc_priv *pDev)
{
	unsigned int rp, wp, asr, bufmax, rrp, rwp;
	struct grtc_regs *regs = pDev->regs;

	FUNCDBG();

	rp = READ_REG(&regs->rp);
	asr = READ_REG(&regs->asr);
	bufmax = (asr & GRTC_ASR_RXLEN) >> GRTC_ASR_RXLEN_BIT;
	bufmax = (bufmax+1) << 10; /* Convert from 1kbyte blocks into bytes */
	wp = READ_REG(&regs->wp);

	/* Relative rp and wp */
	rrp = rp - (asr & GRTC_ASR_BUFST);
	rwp = wp - (asr & GRTC_ASR_BUFST);

	return grtc_hw_data_avail(rrp,rwp,bufmax);
}

static void *grtc_memalign(unsigned int boundary, unsigned int length, void *realbuf)
{
	*(int *)realbuf = (int)grlib_malloc(length+(~GRTC_ASR_BUFST)+1);
	DBG("GRTC: Alloced %d (0x%x) bytes, requested: %d\n",length+(~GRTC_ASR_BUFST)+1,length+(~GRTC_ASR_BUFST)+1,length);
	return (void *)(((*(unsigned int *)realbuf)+(~GRTC_ASR_BUFST)+1) & ~(boundary-1));
}

static int grtc_start(struct grtc_priv *pDev)
{
	struct grtc_regs *regs = pDev->regs;
	unsigned int tmp;

	if ( !pDev->buf || (((unsigned int)pDev->buf & ~GRTC_ASR_BUFST) != 0) ||
	     (pDev->len>(1024*0x100)) || (pDev->len<1024) || ((pDev->len & (1024-1)) != 0) 
	   ) {
		DBG("GRTC: start: buffer not properly allocated(0x%x,0x%x,0x%x,0x%x)\n",pDev->buf,pDev->len,((unsigned int)pDev->buf & ~GRTC_ASR_BUFST),(pDev->len & ~(1024-1)));
		return RTEMS_NO_MEMORY;
	}

	memset(pDev->buf,0,pDev->len);

	/* Software init */
	pDev->overrun_condition = 0;
#ifdef DEBUG_ERROR
	pDev->last_error_cnt = 0;
	memset(&pDev->last_error[0],0,128*sizeof(int));
#endif
	memset(&pDev->stats,0,sizeof(struct grtc_ioc_stats));

	/* Reset the receiver */
	regs->cor = GRTC_SEB | GRTC_COR_CRST;
	if ( READ_REG(&regs->cor) & GRTC_COR_CRST ){
		/* Reset Failed */
		DBG("GRTC: start: Reseting receiver failed\n");
		return RTEMS_IO_ERROR;
	}

	/* make sure the RX semaphore is in the correct state when starting.
	 * In case of a previous overrun condition it could be in incorrect
	 * state (where rtems_semaphore_flush was used).
	 */
	rtems_semaphore_obtain(pDev->sem_rx, RTEMS_NO_WAIT, 0);

	/* Set operating modes */
	tmp = 0;
	if ( pDev->config.psr_enable )
		tmp |= GRTC_GCR_PSR;
	if ( pDev->config.nrzm_enable )
		tmp |= GRTC_GCR_NRZM;
	if ( pDev->config.pss_enable )
		tmp |= GRTC_GCR_PSS;
	regs->gctrl = GRTC_SEB | tmp;

	/* Clear any pending interrupt */
	tmp = READ_REG(&regs->pir);
	regs->picr = GRTC_INT_ALL;

	/* Unmask only the Overrun interrupt */
	regs->imr = GRTC_INT_OV;

	/* Set up DMA registers
	 * 1. Let hardware know about our DMA area (size and location)
	 * 2. Set DMA read/write posistions to zero.
	 */
	regs->asr = (unsigned int)pDev->buf_remote | ((pDev->len>>10)-1);
	regs->rp = (unsigned int)pDev->buf_remote;

	/* Mark running before enabling the receiver, we could receive 
	 * an interrupt directly after enabling the receiver and it would 
	 * then interpret the interrupt as spurious (see interrupt handler)
	 */
	pDev->running = 1;

	/* Enable receiver */
	regs->cor = GRTC_SEB | GRTC_COR_RE;

	DBG("GRTC: STARTED\n");

	return 0;
}

static void grtc_stop(struct grtc_priv *pDev, int overrun)
{
	struct grtc_regs *regs = pDev->regs;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&pDev->devlock, irqflags);

	/* Disable the receiver */
	regs->cor = GRTC_SEB;

	/* disable all interrupts and clear them */
	regs->imr = 0;
	READ_REG(&regs->pir);
	regs->picr = GRTC_INT_ALL;

	DBG("GRTC: STOPPED\n");

	if (overrun) {
		pDev->overrun_condition = 1;
	} else {
		pDev->running = 0;
	}

	SPIN_UNLOCK_IRQ(&pDev->devlock, irqflags);

	/* Flush semaphores in case a thread is stuck waiting for CLTUs (RX data) */
	rtems_semaphore_flush(pDev->sem_rx);
}

/* Wait until 'count' bytes are available in receive buffer, or until 
 * the timeout expires. 
 */
static int grtc_wait_data(struct grtc_priv *pDev, int count, rtems_interval timeout)
{
	int avail;
	int ret;
	SPIN_IRQFLAGS(irqflags);

	FUNCDBG();

	if ( count < 1 )
		return 0;

	SPIN_LOCK_IRQ(&pDev->devlock, irqflags);

	/* Enable interrupts when receiving CLTUs, Also clear old pending CLTUs store
	 * interrupts.
	 */
	pDev->regs->picr = GRTC_INT_CS;
	pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRTC_INT_CS;
	
	avail = grtc_data_avail(pDev);
	if ( avail < count ) {
		/* Wait for interrupt. */

		SPIN_UNLOCK_IRQ(&pDev->devlock, irqflags);

		if ( timeout == 0 ){
			timeout = RTEMS_NO_TIMEOUT;
		}
		ret = rtems_semaphore_obtain(pDev->sem_rx,RTEMS_WAIT,timeout);
		/* RTEMS_SUCCESSFUL  = interrupt signaled data is available
		 * RTEMS_TIMEOUT     = timeout expired, probably not enough data available
		 * RTEMS_UNSATISFIED = driver has been closed or an error (overrun) occured
		 *                     which should cancel this operation.
		 * RTEMS_OBJECT_WAS_DELETED, RTEMS_INVALID_ID = driver error.
		 */
		SPIN_LOCK_IRQ(&pDev->devlock, irqflags);
	}else{
		ret = RTEMS_SUCCESSFUL;	
	}

	/* Disable interrupts when receiving CLTUs */
	pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~GRTC_INT_CS;

	SPIN_UNLOCK_IRQ(&pDev->devlock, irqflags);

	return ret;
}

static rtems_device_driver grtc_open(
	rtems_device_major_number major, 
	rtems_device_minor_number minor, 
	void *arg)
{
	struct grtc_priv *pDev;
	struct drvmgr_dev *dev;

	FUNCDBG();

	if ( drvmgr_get_dev(&grtc_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtc_priv *)dev->priv;

	/* Wait until we get semaphore */
	if ( rtems_semaphore_obtain(grtc_dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL ){
		return RTEMS_INTERNAL_ERROR;
	}

	/* Is device in use? */
	if ( pDev->open ){
		rtems_semaphore_release(grtc_dev_sem);
		return RTEMS_RESOURCE_IN_USE;
	}

	/* Mark device taken */
	pDev->open = 1;
	
	rtems_semaphore_release(grtc_dev_sem);

	DBG("grtc_open: OPENED minor %d (pDev: 0x%x)\n",pDev->minor,(unsigned int)pDev);

	/* Set defaults */
	pDev->buf = NULL;
	pDev->_buf = NULL;
	pDev->buf_custom = 0;
	pDev->buf_remote = 0;
	pDev->len = 0;
	pDev->timeout = 0; /* no timeout */
	pDev->blocking = 0; /* polling mode */
	pDev->mode = GRTC_MODE_RAW; /* Always default to Raw mode */
	pDev->ready.head = NULL;
	pDev->ready.tail = NULL;
	pDev->ready.cnt = 0;

	pDev->running = 0;
	pDev->overrun_condition = 0;

	memset(&pDev->config,0,sizeof(pDev->config));

	/* The core has been reset when we execute here, so it is possible
	 * to read out defualts from core.
	 */
	grtc_hw_get_defaults(pDev,&pDev->config);

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grtc_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grtc_priv *pDev;
	struct drvmgr_dev *dev; 

	FUNCDBG();

	if ( drvmgr_get_dev(&grtc_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtc_priv *)dev->priv;

	if ( pDev->running ){
		grtc_stop(pDev, 0);
	}

	/* Reset core */
	grtc_hw_reset(pDev);
	
	/* Mark not open */
	pDev->open = 0;
	
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grtc_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grtc_priv *pDev;
	struct drvmgr_dev *dev;
	int count;
	int left;
	int timedout;
	int err;
	rtems_interval timeout;
	rtems_libio_rw_args_t *rw_args;

	FUNCDBG();

	if ( drvmgr_get_dev(&grtc_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtc_priv *)dev->priv;

	if ( !pDev->running && !pDev->overrun_condition ) {
		return RTEMS_RESOURCE_IN_USE;
	}

	if ( pDev->mode != GRTC_MODE_RAW ) {
		return RTEMS_NOT_DEFINED;
	}

	rw_args = (rtems_libio_rw_args_t *) arg;
	left = rw_args->count;
	timedout = 0;
	timeout = pDev->timeout;

read_from_buffer:
	/* Read maximally rw_args->count bytes from receive buffer */
	count = grtc_hw_read_try(pDev,rw_args->buffer,left);
	
	left -= count;

	DBG("READ %d bytes from DMA, left: %d\n",count,left);

	if ( !timedout && !pDev->overrun_condition && ((count < 1) || ((count < rw_args->count) && (pDev->blocking == GRTC_BLKMODE_COMPLETE))) ){
		/* didn't read anything (no data available) or we want to wait for all bytes requested.
		 * 
		 * Wait for data to arrive only in blocking mode
		 */
		if ( pDev->blocking ) {
			if ( (err=grtc_wait_data(pDev,left,timeout)) != RTEMS_SUCCESSFUL ){
				/* Some kind of error, closed, overrun etc. */
				if ( err == RTEMS_TIMEOUT ){
					/* Got a timeout, we try to read as much as possible */
					timedout = 1;
					goto read_from_buffer;
				}
				return err;
			}
			goto read_from_buffer;
		}
		/* Non-blocking mode and no data read. */
		return RTEMS_TIMEOUT;
	}

	/* Tell caller how much was read. */

	DBG("READ returning %d bytes, left: %d\n",rw_args->count-left,left);

	rw_args->bytes_moved = rw_args->count - left;
	if ( rw_args->bytes_moved == 0 ) {
		if ( pDev->overrun_condition ) {
			/* signal to the user that overrun has happend when
			 * no more data can be read out.
			 */
			return RTEMS_IO_ERROR;
		}
		return RTEMS_TIMEOUT;
	}

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grtc_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	FUNCDBG();
	return RTEMS_NOT_IMPLEMENTED;
}

static int grtc_pool_add_frms(struct grtc_frame *frms)
{
	struct grtc_frame *frm, *next;
	
	/* Add frames to pools */
	frm = frms;
	while(frm){

		if ( !frm->pool ) {
			/* */
			DBG("GRTC: Frame not assigned to a pool\n");
			return -1;
		}
		next = frm->next; /* Remember next frame to process */
		
		DBG("GRTC: adding frame 0x%x to pool %d (%d)\n",frm,frm->pool->frame_len,frm->pool->frame_cnt);
		
		/* Insert Frame into pool */
		frm->next = frm->pool->frms;
		frm->pool->frms = frm;
		frm->pool->frame_cnt++;

		frm = next;
	}
	
	return 0;
}

static struct grtc_frame *grtc_pool_get_frm(struct grtc_priv *pDev, int frame_len, int *error)
{
	struct grtc_frame *frm;
	struct grtc_frame_pool *pool;
	int i;
	
	/* Loop through all pools until a pool is found
	 * with a matching (or larger) frame length
	 */
	pool = pDev->pools;
	for (i=0; i<pDev->pool_cnt; i++,pool++) {
		if ( pool->frame_len >= frame_len ) {
			/* Found a good pool ==> get frame */
			frm = pool->frms;
			if ( !frm ) {
				/* not enough frames available for this 
				 * frame length, we try next
				 *
				 * If this is a severe error add your handling
				 * code here.
				 */
#if 0
				if ( error )
					*error = 0;
				return 0;
#endif
				continue;
			}
			
			/* Got a frame, the frame is taken out of the 
			 * pool for usage.
			 */
			pool->frms = frm->next;
			pool->frame_cnt--;
			return frm;
		}
	}
	
	if ( error )
		*error = 1;

	/* Didn't find any frames */
	return NULL;
}

/* Return number of bytes processed, Stops at the first occurance 
 * of the pattern given in 'pattern'
 */
static int grtc_scan(unsigned short *src, int max, unsigned char pattern, int *found)
{
	unsigned short tmp = 0;
	unsigned int left = max;

	while ( (left>1) && (((tmp=*src) & 0x00ff) != pattern) ) {
		src++;
		left-=2;
	}
	if ( (tmp & 0xff) == pattern ) {
		*found = 1;
	} else {
		*found = 0;
	}
	return max-left;
}

static int grtc_copy(unsigned short *src, unsigned char *buf, int cnt)
{
	unsigned short tmp;
	int left = cnt;
	
	while ( (left>0) && ((((tmp=*src) & 0x00ff) == 0x00) || ((tmp & 0x00ff) == 0x01)) ) {
		*buf++ = tmp>>8;
		src++;
		left--;
	}
	
	return cnt-left;
}


static int grtc_hw_find_frm(struct grtc_priv *pDev)
{
	struct grtc_regs *regs = pDev->regs;
	unsigned int rp, wp, asr, bufmax, rrp, rwp;
	unsigned int upper, lower;
	unsigned int count, cnt;
	int found;

	FUNCDBG();
	
	rp = READ_REG(&regs->rp);
	asr = READ_REG(&regs->asr);
	wp = READ_REG(&regs->wp);

	/* Quick Check for most common case where Start of frame is at next 
	 * data byte.
	 */	
	if ( rp != wp ) {
		/* At least 1 byte in buffer */
		if ( ((*(unsigned short *)((rp - (unsigned int)pDev->buf_remote) + (unsigned int)pDev->buf)) & 0x00ff) == 0x01 ) {
			return 0;
		}
	}
	
	bufmax = (asr & GRTC_ASR_RXLEN) >> GRTC_ASR_RXLEN_BIT;
	bufmax = (bufmax+1) << 10; /* Convert from 1kbyte blocks into bytes */
	
	/* Relative rp and wp */
	rrp = rp - (asr & GRTC_ASR_BUFST);
	rwp = wp - (asr & GRTC_ASR_BUFST);
	
	lower = grtc_hw_data_avail_lower(rrp,rwp,bufmax);
	upper = grtc_hw_data_avail_upper(rrp,rwp,bufmax);
	
	DBG("grtc_hw_find_frm: AVAIL: Lower: %d, Upper: %d\n",lower,upper);
	DBG("grtc_hw_find_frm: rp: 0x%x, rrp: 0x%x, wp: 0x%x, rwp: 0x%x, bufmax: %d\n, start: 0x%x\n",
		rp,rrp,wp,rwp,bufmax,pDev->buf_remote);
	
	if ( (upper+lower) == 0 )
		return 1;

	/* Count bytes will be read */
	count = 0;
	found = 0;
	
	/* Read from upper part of data buffer */
	if ( upper > 0 ){
		cnt = grtc_scan((unsigned short *)((rp - (unsigned int)pDev->buf_remote) + (unsigned int)pDev->buf), upper, 0x01, &found);
		count = cnt;
		if ( found ) {
			DBG("grtc_hw_find_frm: SCANNED upper %d bytes until found\n",cnt);
			goto out;
		}
		
		DBG("grtc_hw_find_frm: SCANNED all upper %d bytes, not found\n",cnt);
	}
	
	/* Read from lower part of data buffer */
	if ( lower > 0 ){
		cnt = grtc_scan((unsigned short *)pDev->buf, lower, 0x01, &found);
		count += cnt;

		if ( found ) {
			DBG("grtc_hw_find_frm: SCANNED lower %d bytes until found\n",cnt);
			goto out;
		}
		
		DBG("grtc_hw_find_frm: SCANNED all lower %d bytes, not found\n",cnt);
	}

out:
	/* Update hardware RP pointer to tell hardware about new space available */
	if ( count > 0 ) {
		if ( (rp+count) >= ((asr&GRTC_ASR_BUFST)+bufmax) ){
			regs->rp = (rp+count-bufmax);
		} else {
			regs->rp = rp+count;
		}
	}
	if ( found )
		return 0;
	return 1;

}

static int grtc_check_ending(unsigned short *src, int max, int end)
{
	while ( max > 0 ) {
		/* Check Filler */
		if ( *src != 0x5500 ) {
			/* Filler is wrong */
			return -1;
		}
		src++;
		max-=2;
	}
	
	/* Check ending (at least */
	if ( end ) {
		if ( (*src & 0x00ff) != 0x02 ) {
			return -1;
		}
	}

	return 0;
}

static int grtc_hw_check_ending(struct grtc_priv *pDev, int max)
{
	struct grtc_regs *regs = pDev->regs;
	unsigned int rp, wp, asr, bufmax, rrp, rwp;
	unsigned int upper, lower;
	unsigned int count, cnt, left;

	FUNCDBG();

	if ( max < 1 )
		return 0;
	max = max*2;
	max += 2; /* Check ending also (2 byte extra) */

	rp = READ_REG(&regs->rp);
	asr = READ_REG(&regs->asr);
	bufmax = (asr & GRTC_ASR_RXLEN) >> GRTC_ASR_RXLEN_BIT;
	bufmax = (bufmax+1) << 10; /* Convert from 1kbyte blocks into bytes */
	wp = READ_REG(&regs->wp);

	/* Relative rp and wp */
	rrp = rp - (asr & GRTC_ASR_BUFST);
	rwp = wp - (asr & GRTC_ASR_BUFST);

	lower = grtc_hw_data_avail_lower(rrp,rwp,bufmax);
	upper = grtc_hw_data_avail_upper(rrp,rwp,bufmax);
	
	DBG("grtc_hw_check_ending: AVAIL: Lower: %d, Upper: %d\n",lower,upper);
	DBG("grtc_hw_check_ending: rp: 0x%x, rrp: 0x%x, wp: 0x%x, rwp: 0x%x, bufmax: %d\n, start: 0x%x\n",
		rp,rrp,wp,rwp,bufmax,pDev->buf_remote);
	
	if ( (upper+lower) < max )
		return 0;
	
	/* Count bytes will be read */
	count = max;
	left = count;
	
	/* Read from upper part of data buffer */
	if ( upper > 0 ){
		if ( left <= upper ){
			cnt = left;
			if ( grtc_check_ending((unsigned short *)((rp-(unsigned int)pDev->buf_remote)+(unsigned int)pDev->buf), cnt-2, 1) ) {
				return -1;
			}
		}else{
			cnt = upper;	/* Read all upper data available */
			if ( grtc_check_ending((unsigned short *)((rp-(unsigned int)pDev->buf_remote)+(unsigned int)pDev->buf), cnt, 0) ) {
				return -1;
			}
		}
		left -= cnt;
	}
	
	/* Read from lower part of data buffer */
	if ( left > 0 ){
		cnt = left;
		if ( grtc_check_ending((unsigned short *)pDev->buf, cnt-2, 1) ) {
			return -1;
		}
		left -= cnt;
	}

	/* Update hardware RP pointer to tell hardware about new space available */
	if ( (rp+count) >= ((asr&GRTC_ASR_BUFST)+bufmax) ){
		regs->rp = (rp+count-bufmax);
	} else {
		regs->rp = rp+count;
	}

	return 0;	
}

/* Copies Data from DMA area to buf, the control bytes are stripped. For
 * every data byte, in the DMA area, one control byte is stripped.
 */
static int grtc_hw_copy(struct grtc_priv *pDev, unsigned char *buf, int max, int partial)
{
	struct grtc_regs *regs = pDev->regs;
	unsigned int rp, wp, asr, bufmax, rrp, rwp;
	unsigned int upper, lower;
	unsigned int count, cnt, left;
	int ret, tot, tmp;

	FUNCDBG();

	if ( max < 1 )
		return 0;

	rp = READ_REG(&regs->rp);
	asr = READ_REG(&regs->asr);
	bufmax = (asr & GRTC_ASR_RXLEN) >> GRTC_ASR_RXLEN_BIT;
	bufmax = (bufmax+1) << 10; /* Convert from 1kbyte blocks into bytes */
	wp = READ_REG(&regs->wp);

	/* Relative rp and wp */
	rrp = rp - (asr & GRTC_ASR_BUFST);
	rwp = wp - (asr & GRTC_ASR_BUFST);

	lower = grtc_hw_data_avail_lower(rrp,rwp,bufmax) >> 1;
	upper = grtc_hw_data_avail_upper(rrp,rwp,bufmax) >> 1;

	DBG("grtc_hw_copy: AVAIL: Lower: %d, Upper: %d\n",lower,upper);
	DBG("grtc_hw_copy: rp: 0x%x, rrp: 0x%x, wp: 0x%x, rwp: 0x%x, bufmax: %d\n, start: 0x%x\n",
		rp,rrp,wp,rwp,bufmax,pDev->buf_remote);

	if ( (upper+lower) == 0 || (!partial && ((upper+lower)<max) ) )
		return 0;

	/* Count bytes will be read */
	count = (upper+lower) > max ? max : (upper+lower);
	left = count;
	tot = 0;

	/* Read from upper part of data buffer */
	if ( upper > 0 ){
		if ( left < upper ){
			cnt = left;
		}else{
			cnt = upper;	/* Read all upper data available */
		}
		DBG("grtc_hw_copy: COPYING %d from upper\n",cnt);
		if ( (tot=grtc_copy((unsigned short *)((rp-(unsigned int)pDev->buf_remote)+(unsigned int)pDev->buf), buf, cnt)) != cnt ) {
			/* Failed to copy due to an receive error */
			DBG("grtc_hw_copy(upper): not all in DMA buffer (%d)\n",tot);
			count = tot;
			ret = -1;
			goto out;
		}
		buf += cnt;
		left -= cnt;
	}
	
	/* Read from lower part of data buffer */
	if ( left > 0 ){
		if ( left < lower ){
			cnt = left;
		}else{
			cnt = lower;	/* Read all lower data available */
		}
		DBG("grtc_hw_copy: COPYING %d from lower\n",cnt);
		if ( (tmp=grtc_copy((unsigned short *)pDev->buf, buf, cnt)) != cnt ) {
			/* Failed to copy due to an receive error */
			DBG("grtc_hw_copy(lower): not all in DMA buffer (%d)\n",tot);
			count = tot+tmp;
			ret = -1;
			goto out;
		}
		buf += cnt;
		left -= cnt;
	}
	ret = count;

out:
	count = count*2;
	/* Update hardware RP pointer to tell hardware about new space available */
	if ( (rp+count) >= ((asr&GRTC_ASR_BUFST)+bufmax) ){
		regs->rp = (rp+count-bufmax);
	} else {
		regs->rp = rp+count;
	}

	return ret;
}

#ifdef DEBUG_ERROR
void grtc_log_error(struct grtc_priv *pDev, int err)
{
	/* Stop Receiver */
	*(volatile unsigned int *)&pDev->regs->cor = 0x55000000;
	*(volatile unsigned int *)&pDev->regs->cor = 0x55000000;
	pDev->last_error[pDev->last_error_cnt] = err;
	if ( ++pDev->last_error_cnt > 128 )
		pDev->last_error_cnt = 0;
}
#endif

/* Read one frame from DMA buffer 
 * 
 * Return Values
 *  Zero - nothing more to process
 *  1 - more to process, no free frames
 *  2 - more to process, frame received
 *  negative - more to process, frame dropped
 */
static int process_dma(struct grtc_priv *pDev)
{
	int ret, err;
	int left, total_len;
	unsigned char *dst;
	struct grtc_frame *frm;

	switch( pDev->frame_state ) {
		case FRM_STATE_NONE:
		DBG2("FRAME_STATE_NONE\n");
	
		/* Find Start of next frame by searching for 0x01 */
		ret = grtc_hw_find_frm(pDev);
		if ( ret != 0 ) {
			/* Frame start not found */
			return 0;
		}
		
		/* Start of frame found, Try to copy header */
		pDev->frm = NULL;
		pDev->frame_state = FRM_STATE_HDR;

		case FRM_STATE_HDR:
		DBG2("FRAME_STATE_HDR\n");
		
		/* Wait for all of header to be in place by setting partial to 0 */
		ret = grtc_hw_copy(pDev, (unsigned char *)pDev->hdr, 5, 0);
		if ( ret < 0 ) {
			/* Error copying header, restart scanning for new frame */
			DEBUG_ERR_LOG(pDev,1);
			pDev->stats.err++;
			pDev->stats.err_hdr++;
			DBG("FRAME_STATE_HDR: copying failed %d\n",ret);
			pDev->frame_state = FRM_STATE_NONE;
			return -1;
		} else if ( ret != 5 ) {
			DBG("FRAME_STATE_HDR: no header (%d)\n",ret);
			/* Not all bytes available, come back later */
			return 0;
		}

		/* The complete header has been copied, parse it */
		pDev->frmlen = (((unsigned short *)pDev->hdr)[1] & 0x3ff)+1;
		if ( pDev->frmlen < 5 ) {
			/* Error: frame length is not correct */
			pDev->stats.err++;
			pDev->stats.err_hdr++;
			DBG("FRAME_STATE_HDR: frame length error: %d\n", pDev->frmlen);
			pDev->frame_state = FRM_STATE_NONE;
			return -1;
		}
		pDev->frame_state = FRM_STATE_ALLOC;

		case FRM_STATE_ALLOC:
		DBG2("FRAME_STATE_ALLOC\n");
		/* Header has been read, allocate a frame to put payload and header into */
		
		/* Allocate Frame matching Frame length */
		err = 0;
		frm = grtc_pool_get_frm(pDev,pDev->frmlen,&err);
		if ( !frm ) {
			/* Couldn't find frame  */
			DEBUG_ERR_LOG(pDev,2);
			pDev->stats.dropped++;
			DBG2("No free frames\n");
			if ( err == 0 ){
				/* Frame length exist in pool configuration, but no
				 * frames are available for that frame length.
				 */
				DEBUG_ERR_LOG(pDev,3);
				pDev->stats.dropped_no_buf++;
				return 1;
			} else {
				/* Frame length of incoming frame is larger than the
				 * frame length in any of the configured frame pools.
				 * 
				 * This may be because of an corrupt header. We simply
				 * scan for the end of frame marker in the DMA buffer
				 * so we can drop the frame.
				 */
				DEBUG_ERR_LOG(pDev,4);
				pDev->stats.dropped_too_long++;
				pDev->frame_state = FRM_STATE_NONE;
				return -2;
			}
		}
		frm->len = 5; /* Only header currenlty in frame */

		/* Copy Frame Header into frame structure */
		((unsigned char*)&frm->hdr)[0] = ((unsigned char*)pDev->hdr)[0];
		((unsigned char*)&frm->hdr)[1] = ((unsigned char*)pDev->hdr)[1];
		((unsigned char*)&frm->hdr)[2] = ((unsigned char*)pDev->hdr)[2];
		((unsigned char*)&frm->hdr)[3] = ((unsigned char*)pDev->hdr)[3];
		((unsigned char*)&frm->hdr)[4] = ((unsigned char*)pDev->hdr)[4];

		/* Calc Total and Filler byte count in frame */
		total_len = pDev->frmlen / 7;
		total_len = total_len * 7;
		if ( pDev->frmlen != total_len )
			total_len += 7;

		pDev->filler = total_len - pDev->frmlen;

		pDev->frame_state = FRM_STATE_PAYLOAD;
		pDev->frm = frm;

		case FRM_STATE_PAYLOAD:
		DBG2("FRAME_STATE_PAYLOAD\n");
		/* Parts of payload and the complete header has been read */
		frm = pDev->frm;

		dst = (unsigned char *)&frm->data[frm->len-5];
		left = pDev->frmlen-frm->len;

		ret = grtc_hw_copy(pDev,dst,left,1);
		if ( ret < 0 ) {
			DEBUG_ERR_LOG(pDev,5);
			/* Error copying header, restart scanning for new frame */
			pDev->frame_state = FRM_STATE_NONE;
			frm->next = NULL;
			grtc_pool_add_frms(frm);
			pDev->frm = NULL;
			pDev->stats.err++;
			pDev->stats.err_payload++;
			return -1;
		} else if ( ret != left ) {
			/* Not all bytes available, come back later */
			frm->len += ret;
			return 0;
		}
		frm->len += ret;
		pDev->frame_state = FRM_STATE_FILLER;

		case FRM_STATE_FILLER:
		DBG2("FRAME_STATE_FILLER\n");
		/* check filler data */
		frm = pDev->frm;

		ret = grtc_hw_check_ending(pDev,pDev->filler);
		if ( ret != 0 ) {
			/* Error in frame, drop frame */
			DEBUG_ERR_LOG(pDev,6);
			pDev->frame_state = FRM_STATE_NONE;
			frm->next = NULL;
			grtc_pool_add_frms(frm);
			pDev->frm = NULL;
			pDev->stats.err++;
			pDev->stats.err_ending++;
			return -1;
		}

		/* A complete frame received, put it into received frame queue */
		if ( pDev->ready.head ) {
			/* Queue not empty */
			pDev->ready.tail->next = frm;
		} else {
			/* Queue empty */
			pDev->ready.head = frm;
		}
		pDev->ready.tail = frm;
		frm->next = NULL;
		pDev->ready.cnt++;
		pDev->stats.frames_recv++;

		pDev->frame_state = FRM_STATE_NONE;
		frm->next = NULL;
		return 2;

#if 0
		case FRM_STATE_DROP:
		DBG2("FRAME_STATE_DROP\n");
		break;
#endif

		default:
		printk("GRTC: internal error\n");
		pDev->frame_state = FRM_STATE_NONE;
		break;
	}
	
	return 0;
}

static rtems_device_driver grtc_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grtc_priv *pDev;
	struct drvmgr_dev *dev;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *)arg;
	unsigned int *data = ioarg->buffer;
	int status,frm_len,i,ret;
	struct grtc_ioc_buf_params *buf_arg;
	struct grtc_ioc_config *cfg;
	struct grtc_ioc_hw_status *hwregs;
	struct grtc_ioc_pools_setup *pocfg;
	struct grtc_ioc_assign_frm_pool *poassign;
	struct grtc_frame *frm, *frms;
	struct grtc_frame_pool *pool;
	struct grtc_list *frmlist;
	struct grtc_ioc_stats *stats;
	unsigned int mem;
	IRQ_LOCAL_DECLARE(oldLevel);

	FUNCDBG();

	if ( drvmgr_get_dev(&grtc_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtc_priv *)dev->priv;

	if (!ioarg)
		return RTEMS_INVALID_NAME;

	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
		case GRTC_IOC_START:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */
		}
		if ( (status=grtc_start(pDev)) != RTEMS_SUCCESSFUL ){
			return status;
		}
		/* Register ISR and Unmask interrupt */
		drvmgr_interrupt_register(pDev->dev, 0, "grtc", grtc_interrupt, pDev);

		/* Read and write are now open... */
		break;

		case GRTC_IOC_STOP:
		if ( !pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}
		drvmgr_interrupt_unregister(pDev->dev, 0, grtc_interrupt, pDev);
		grtc_stop(pDev, 0);
		break;

		case GRTC_IOC_ISSTARTED:
		if ( !pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		} else if ( pDev->overrun_condition ) {
			return RTEMS_IO_ERROR;
		}
		break;

		case GRTC_IOC_SET_BLOCKING_MODE:
		if ( (unsigned int)data > GRTC_BLKMODE_COMPLETE ) {
			return RTEMS_INVALID_NAME;
		}
		DBG("GRTC: Set blocking mode: %d\n",(unsigned int)data);
		pDev->blocking = (unsigned int)data;
		break;

		case GRTC_IOC_SET_TIMEOUT:
		DBG("GRTC: Timeout: %d\n",(unsigned int)data);
		pDev->timeout = (rtems_interval)data;
		break;

		case GRTC_IOC_SET_BUF_PARAM:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */
		}

		buf_arg = (struct grtc_ioc_buf_params *)data;
		if ( !buf_arg ) {
			return RTEMS_INVALID_NAME;
		}

		DBG("GRTC: IOC_SET_BUF_PARAM: Len: 0x%x, Custom Buffer: 0x%x\n",buf_arg->length,buf_arg->custom_buffer);

		/* Check alignment need, skip bit 0 since that bit only indicates remote address or not */
		if ( (unsigned int)buf_arg->custom_buffer & (~GRTC_BUF_MASK) & (~0x1) ) {
			return RTEMS_INVALID_NAME;
		}

		if ( buf_arg->length > 0x100 ){
			DBG("GRTC: Too big buffer requested\n");
			return RTEMS_INVALID_NAME;
		}

		/* If current buffer allocated by driver we must free it */
		if ( !pDev->buf_custom && pDev->buf ){
			free(pDev->_buf);
			pDev->_buf = NULL;
		}
		pDev->buf = NULL;
		pDev->len = buf_arg->length*1024;

		if (pDev->len <= 0)
			break;
		mem = (unsigned int)buf_arg->custom_buffer;
		pDev->buf_custom = mem;

		if (mem & 1) {
			/* Remote address given, the address is as the GRTC
			 * core looks at it. Translate the base address into
			 * an address that the CPU can understand.
			 */
			pDev->buf_remote = (void *)(mem & ~0x1);
			drvmgr_translate_check(pDev->dev, DMAMEM_TO_CPU,
						(void *)pDev->buf_remote,
						(void **)&pDev->buf,
						pDev->len);
		} else {
			if (mem == 0) {
				pDev->buf = grtc_memalign((~GRTC_ASR_BUFST)+1,pDev->len,&pDev->_buf);
				DBG("grtc_ioctl: SETBUF: new buf: 0x%x(0x%x), Len: %d\n",pDev->buf,pDev->_buf,pDev->len);
				if (!pDev->buf){
					pDev->len = 0;
					pDev->buf_custom = 0;
					pDev->_buf = NULL;
					pDev->buf_remote = 0;
					DBG("GRTC: Failed to allocate memory\n");
					return RTEMS_NO_MEMORY;
				}
			} else{
				pDev->buf = buf_arg->custom_buffer;
			}

			/* Translate into a remote address so that GRTC core
			 * on a remote AMBA bus (for example over the PCI bus)
			 * gets a valid address
			 */
			drvmgr_translate_check(pDev->dev, CPUMEM_TO_DMA,
						(void *)pDev->buf,
						(void **)&pDev->buf_remote,
						pDev->len);
		}
		break;

		case GRTC_IOC_GET_BUF_PARAM:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */
		}

		buf_arg = (struct grtc_ioc_buf_params *)data;
		if ( !buf_arg ) {
			return RTEMS_INVALID_NAME;
		}

		buf_arg->length = pDev->len >> 10; /* Length in 1kByte blocks */
		if ( pDev->buf_custom )
			buf_arg->custom_buffer =(void *)pDev->buf;
		else
			buf_arg->custom_buffer = 0; /* Don't reveal internal driver buffer */
		break;

		case GRTC_IOC_SET_CONFIG:
		cfg = (struct grtc_ioc_config *)data;
		if ( !cfg ) {
			return RTEMS_INVALID_NAME;
		}
		
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}

		pDev->config = *cfg;
		break;

		case GRTC_IOC_GET_CONFIG:
		cfg = (struct grtc_ioc_config *)data;
		if ( !cfg ) {
			return RTEMS_INVALID_NAME;
		}

		*cfg = pDev->config;
		break;

		case GRTC_IOC_GET_HW_STATUS:
		hwregs = (struct grtc_ioc_hw_status *)data;
		if ( !hwregs ) {
			return RTEMS_INVALID_NAME;
		}
		/* We disable interrupt on the local CPU in order to get a
		 * snapshot of the registers.
		 */
		IRQ_LOCAL_DISABLE(oldLevel);
		hwregs->sir	= READ_REG(&pDev->regs->sir);
		hwregs->far	= READ_REG(&pDev->regs->far);
		hwregs->clcw1	= READ_REG(&pDev->regs->clcw1);
		hwregs->clcw2	= READ_REG(&pDev->regs->clcw2);
		hwregs->phir	= READ_REG(&pDev->regs->phir);
		hwregs->str	= READ_REG(&pDev->regs->str);
		IRQ_LOCAL_ENABLE(oldLevel);
		break;

		case GRTC_IOC_GET_STATS:
		stats = (struct grtc_ioc_stats *)data;
		if ( !stats ) {
			return RTEMS_INVALID_NAME;
		}
		memcpy(stats,&pDev->stats,sizeof(struct grtc_ioc_stats));
		break;

		case GRTC_IOC_CLR_STATS:
		memset(&pDev->stats,0,sizeof(struct grtc_ioc_stats));
		break;
		
		case GRTC_IOC_SET_MODE:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}
		if ( (int)data == GRTC_MODE_FRAME ) {
			pDev->mode = GRTC_MODE_FRAME;
		} else if ( (int)data == GRTC_MODE_RAW ) {
			pDev->mode = GRTC_MODE_RAW;
		} else {
			return RTEMS_INVALID_NAME;
		}
		break;
		
		case GRTC_IOC_POOLS_SETUP:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}
		pocfg = (struct grtc_ioc_pools_setup *)data;
		if ( (pDev->mode != GRTC_MODE_FRAME) || !pocfg ) {
			return RTEMS_INVALID_NAME;
		}
		
		/* Check that list is sorted */
		frm_len = 0;
		for(i=0;i<pocfg->pool_cnt;i++){
			if ( pocfg->pool_frame_len[i] <= frm_len ) {
				return RTEMS_INVALID_NAME;
			}
			frm_len = pocfg->pool_frame_len[i];
		}
		
		/* Ok, we trust user. The pool descriptions are allocated
		 * but not frames, that the user must do self.
		 */
		if ( pDev->pools ) {
			free(pDev->pools);
		}
		pDev->pools = grlib_malloc(pocfg->pool_cnt * sizeof(*pDev->pools));
		if ( !pDev->pools ) {
			pDev->pool_cnt = 0;
			return RTEMS_NO_MEMORY;
		}
		pDev->pool_cnt = pocfg->pool_cnt;
		for (i=0;i<pocfg->pool_cnt;i++) {
			pDev->pools[i].frame_len = pocfg->pool_frame_len[i];
			pDev->pools[i].frame_cnt = 0;
			pDev->pools[i].frms = NULL;
		}
		break;

		case GRTC_IOC_ASSIGN_FRM_POOL:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}

		if ( (pDev->mode != GRTC_MODE_FRAME) ) {
			return RTEMS_INVALID_NAME;
		}

		poassign = (struct grtc_ioc_assign_frm_pool *)data;
		if ( !poassign ) {
			return RTEMS_INVALID_NAME;
		}
		
		/* Find pool to assign the frames to */
		pool = NULL;
		for(i=0; i<pDev->pool_cnt; i++) {
			if ( pDev->pools[i].frame_len == poassign->frame_len ) {
				pool = &pDev->pools[i];
				break;
			}
		}
		if ( !pool ) {
			/* No Pool matching frame length */
			return RTEMS_INVALID_NAME;
		}
		
		/* Assign frames to pool */
		frm = poassign->frames;
		while(frm){
			frm->pool = pool;	/* Assign Frame to pool */
			frm = frm->next;
		}
		break;

		case GRTC_IOC_ADD_BUFF:
		frms = (struct grtc_frame *)data;

		if ( (pDev->mode != GRTC_MODE_FRAME) ) {
			return RTEMS_NOT_DEFINED;
		}
		if ( !frms ) {
			return RTEMS_INVALID_NAME;
		}

		/* Add frames to respicative pools */
		if ( grtc_pool_add_frms(frms) ) {
			return RTEMS_INVALID_NAME;
		}
		break;

		/* Try to read as much data as possible from DMA area and
		 * put it into free frames.
		 *
		 * If receiver is in stopped mode, let user only read previously
		 * received frames.
		 */
		case GRTC_IOC_RECV:

		if ( (pDev->mode != GRTC_MODE_FRAME) ) {
			return RTEMS_NOT_DEFINED;
		}

		while ( pDev->running && ((ret=process_dma(pDev) == 2) || (ret == -1)) ) {
			/* Frame received or dropped, process next frame */
		}

		/* Take frames out from ready queue and put them to user */
		frmlist = (struct grtc_list *)data;
		if ( !frmlist ) {
			return RTEMS_INVALID_NAME;
		}

		frmlist->head = pDev->ready.head;
		frmlist->tail = pDev->ready.tail;
		frmlist->cnt = pDev->ready.cnt;

		/* Empty list */
		pDev->ready.head = NULL;
		pDev->ready.tail = NULL;
		pDev->ready.cnt = 0;

		if ((frmlist->cnt == 0) && pDev->overrun_condition) {
			/* signal to the user that overrun has happend when
			 * no more data can be read out.
			 */
			return RTEMS_IO_ERROR;
		}
		break;

		case GRTC_IOC_GET_CLCW_ADR:
		if ( !data ) {
			return RTEMS_INVALID_NAME;
		}
		*data = (unsigned int)&pDev->regs->clcw1;
		break;

		default:
		return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}

static void grtc_interrupt(void *arg)
{
	struct grtc_priv *pDev = arg;
	struct grtc_regs *regs = pDev->regs;
	unsigned int status;
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Clear interrupt by reading it */
	status = READ_REG(&regs->pisr);

	/* Spurious Interrupt? */
	if ( !pDev->running )
		return;

	if ( status & GRTC_INT_OV ){
		/* Stop core (Disable receiver, interrupts), set overrun condition, 
		 * Flush semaphore if thread waiting for data in grtc_wait_data(). 
		 */
		grtc_stop(pDev, 1);

		/* No need to handle the reset of interrupts, we are still */
		goto out;
	}

	if ( status & GRTC_INT_CS ){
		SPIN_LOCK(&pDev->devlock, irqflags);

		if ( (pDev->blocking==GRTC_BLKMODE_COMPLETE) && pDev->timeout ){
			/* Signal to thread only if enough data is available */
			if ( pDev->wait_for_nbytes > grtc_data_avail(pDev) ){
				/* Not enough data available */
				goto procceed_processing_interrupts;
			}

			/* Enough data is available which means that we should
			 * wake up the thread sleeping.
			 */
		}

		/* Disable further CLTUs Stored interrupts, no point until
		 * thread waiting for them says it want to wait for more.
		 */
		regs->imr = READ_REG(&regs->imr) & ~GRTC_INT_CS;
		SPIN_UNLOCK(&pDev->devlock, irqflags);

		/* Signal Semaphore to wake waiting thread in read() */
		rtems_semaphore_release(pDev->sem_rx);
	}
	
procceed_processing_interrupts:

	if ( status & GRTC_INT_CR ){
	
	}

	if ( status & GRTC_INT_FAR ){
	
	}

	if ( status & GRTC_INT_BLO ){
	
	}

	if ( status & GRTC_INT_RFA ){
	
	}
out:
	if ( status )
		regs->picr = status;
}

static rtems_device_driver grtc_initialize(
  rtems_device_major_number major, 
  rtems_device_minor_number unused,
  void *arg
  )
{
	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'R', 'T', 'C'),
		1,
		RTEMS_FIFO|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&grtc_dev_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	return RTEMS_SUCCESSFUL;
}
