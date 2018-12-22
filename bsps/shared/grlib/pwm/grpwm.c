/*
 *  GRPWM PWM Driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB,
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
#include <grlib/grpwm.h>
#include <grlib/ambapp.h>

#include <grlib/grlib_impl.h>

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#define STATIC
#else
#define DBG(x...) 
#define STATIC static
#endif

/*** REGISTER LAYOUT ***/

/* PWM Channel specific registers */
struct grpwm_pwm_regs {
	volatile unsigned int period;		/* 0x00 */
	volatile unsigned int comp;		/* 0x04 */
	volatile unsigned int dbcomp;		/* 0x08 */
	volatile unsigned int ctrl;		/* 0x0C */
};

/* Core common registers */	
struct grpwm_regs {
	volatile unsigned int ctrl;		/* 0x00 */
	volatile unsigned int scaler;		/* 0x04 */
	volatile unsigned int ipend;		/* 0x08 */
	volatile unsigned int cap1;		/* 0x0C */
	volatile unsigned int cap2;		/* 0x10 */
	volatile unsigned int wctrl;		/* 0x14 */
	int reserved0[2];
	struct grpwm_pwm_regs pwms[8];		/* 0x20 */
	int reserved1[(0x8000-0xA0)/4];		/* 0xA0-0x7FFC */
	volatile unsigned int wram[0x8000/4];	/* 0x8000-0xFFFC */
};

/*** REGISTER BIT LAYOUT ***/

/* CTRL REGISTER - 0x0 */
#define GRPWM_CTRL_EN_BIT	0
#define GRPWM_CTRL_SCSEL_BIT	8
#define GRPWM_CTRL_NOUP_BIT	12
#define GRPWM_CTRL_EN		(1<<GRPWM_CTRL_EN_BIT)
#define GRPWM_CTRL_SCSEL	(0x7<<GRPWM_CTRL_SCSEL_BIT)
#define GRPWM_CTRL_NOUP		(0xff<<GRPWM_CTRL_NOUP_BIT)


/* CAPABILITY1 REGISTER - 0x0C */
#define GRPWM_CAP_NPWM_BIT	0
#define GRPWM_CAP_PBITS_BIT	3
#define GRPWM_CAP_SBITS_BIT	8
#define GRPWM_CAP_NSC_BIT	13
#define GRPWM_CAP_DBB_BIT	16
#define GRPWM_CAP_DBSC_BIT	21
#define GRPWM_CAP_ASY_BIT	22
#define GRPWM_CAP_SYM_BIT	23
#define GRPWM_CAP_SEP_BIT	25
#define GRPWM_CAP_DCM_BIT	27

#define GRPWM_CAP_NPWM	(0x7<<GRPWM_CAP_NPWM_BIT)
#define GRPWM_CAP_PBITS	(0x1f<<GRPWM_CAP_PBITS_BIT)
#define GRPWM_CAP_SBITS	(0x1f<<GRPWM_CAP_SBITS_BIT)
#define GRPWM_CAP_NSC	(0x7<<GRPWM_CAP_NSC_BIT)
#define GRPWM_CAP_DBB	(0x1f<<GRPWM_CAP_DBB_BIT)
#define GRPWM_CAP_DBSC	(1<<GRPWM_CAP_DBSC_BIT)
#define GRPWM_CAP_ASY	(1<<GRPWM_CAP_ASY_BIT)
#define GRPWM_CAP_SYM	(1<<GRPWM_CAP_SYM_BIT)
#define GRPWM_CAP_SEP	(0x3<<GRPWM_CAP_SEP_BIT)
#define GRPWM_CAP_DCM	(1<<GRPWM_CAP_DCM_BIT)

/* CAPABILITY2 REGISTER - 0x10 */
#define GRPWM_CAP2_WPWM_BIT	0
#define GRPWM_CAP2_WDBITS_BIT	1
#define GRPWM_CAP2_WABITS_BIT	6
#define GRPWM_CAP2_WSYNC_BIT	10

#define GRPWM_CAP2_WPWM		(0x1<<GRPWM_CAP2_WPWM_BIT)
#define GRPWM_CAP2_WDBITS	(0x1f<<GRPWM_CAP2_WDBITS_BIT)
#define GRPWM_CAP2_WABITS	(0xf<<GRPWM_CAP2_WABITS_BIT)
#define GRPWM_CAP2_WSYNC	(1<<GRPWM_CAP2_WSYNC_BIT)

/* WAVE FORM CONFIG REGISTER - 0x14 */
#define GRPWM_WCTRL_STOP_BIT		0
#define GRPWM_WCTRL_WSYNC_BIT		16
#define GRPWM_WCTRL_WSEN_BIT		29
#define GRPWM_WCTRL_WSYNCCFG_BIT	30

#define GRPWM_WCTRL_STOP	(0x1fff<<GRPWM_WCTRL_STOP_BIT)
#define GRPWM_WCTRL_WSYNC	(0x1fff<<GRPWM_WCTRL_WSYNC_BIT)
#define GRPWM_WCTRL_WSEN	(0x1<<GRPWM_WCTRL_WSEN_BIT)
#define GRPWM_WCTRL_WSYNCCFG	(0x3<<GRPWM_WCTRL_WSYNCCFG_BIT)


/* PWM CONTROL REGISTER - 0x2C, 0x3C... */
#define GRPWM_PCTRL_EN_BIT	0
#define GRPWM_PCTRL_POL_BIT	1
#define GRPWM_PCTRL_PAIR_BIT	2
#define GRPWM_PCTRL_FIX_BIT	3
#define GRPWM_PCTRL_METH_BIT	6
#define GRPWM_PCTRL_DCEN_BIT	8
#define GRPWM_PCTRL_WEN_BIT	9
#define GRPWM_PCTRL_SCSEL_BIT	10
#define GRPWM_PCTRL_IEN_BIT	13
#define GRPWM_PCTRL_IT_BIT	14
#define GRPWM_PCTRL_ISC_BIT	15
#define GRPWM_PCTRL_DBEN_BIT	21
#define GRPWM_PCTRL_DBSC_BIT	22
#define GRPWM_PCTRL_FLIP_BIT	26

#define GRPWM_PCTRL_EN		(0x1<<GRPWM_PCTRL_EN_BIT)
#define GRPWM_PCTRL_POL		(0x1<<GRPWM_PCTRL_POL_BIT)
#define GRPWM_PCTRL_PAIR	(0x1<<GRPWM_PCTRL_PAIR_BIT)
#define GRPWM_PCTRL_FIX		(0x7<<GRPWM_PCTRL_FIX_BIT)
#define GRPWM_PCTRL_METH	(0x1<<GRPWM_PCTRL_METH_BIT)
#define GRPWM_PCTRL_DCEN	(0x1<<GRPWM_PCTRL_DCEN_BIT)
#define GRPWM_PCTRL_WEN		(0x1<<GRPWM_PCTRL_WEN_BIT)
#define GRPWM_PCTRL_SCSEL	(0x7<<GRPWM_PCTRL_SCSEL_BIT)
#define GRPWM_PCTRL_IEN		(0x1<<GRPWM_PCTRL_IEN_BIT)
#define GRPWM_PCTRL_IT		(0x1<<GRPWM_PCTRL_IT_BIT)
#define GRPWM_PCTRL_ISC		(0x3f<<GRPWM_PCTRL_ISC_BIT)
#define GRPWM_PCTRL_DBEN	(0x1<<GRPWM_PCTRL_DBEN_BIT)
#define GRPWM_PCTRL_DBSC	(0xf<<GRPWM_PCTRL_DBSC_BIT)
#define GRPWM_PCTRL_FLIP	(0xf<<GRPWM_PCTRL_FLIP_BIT)

/*** DRIVER PRIVATE STRUCTURE ***/
struct grpwm_priv {
	struct drvmgr_dev	*dev;
	struct grpwm_regs		*regs;
	char				devName[32];
	int				irq;
	int				open;

	/* Driver implementation */
	char				nscalers;	/* Number of scalers */
	char				wave;		/* If Wave form is available */
	int				wlength;	/* Wave Form RAM Length */
	int				channel_cnt;
	struct grpwm_chan_priv		*channels[8];
	rtems_id			dev_sem;
};

struct grpwm_chan_priv {
	struct grpwm_priv		*common;
	struct grpwm_pwm_regs		*pwmregs;
	/* IRQ */
	int				irqindex;
	void				(*isr)(int channel, void *arg);
	void				*isr_arg;
};

/******************* Driver Manager Part ***********************/

int grpwm_device_init(struct grpwm_priv *priv);
int grpwm_register_io(rtems_device_major_number *m);
static int grpwm_driver_io_registered = 0;
static rtems_device_major_number grpwm_driver_io_major = 0;

int grpwm_init2(struct drvmgr_dev *dev);
int grpwm_init3(struct drvmgr_dev *dev);

struct drvmgr_drv_ops grpwm_ops = 
{
	.init = {NULL, grpwm_init2, grpwm_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id grpwm_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRPWM},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info grpwm_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_GRPWM_ID,		/* Driver ID */
		"GRPWM_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&grpwm_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		0,
	},
	&grpwm_ids[0]
};

void grpwm_register_drv (void)
{
	DBG("Registering GRPWM driver\n");
	drvmgr_drv_register(&grpwm_drv_info.general);
}

int grpwm_init2(struct drvmgr_dev *dev)
{
	struct grpwm_priv *priv;

	DBG("GRPWM[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	priv = dev->priv = grlib_malloc(sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	memset(priv, 0, sizeof(*priv));
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

int grpwm_init3(struct drvmgr_dev *dev)
{
	struct grpwm_priv *priv = dev->priv;
	char prefix[32];
	rtems_status_code status;

	if ( !priv )
		return DRVMGR_FAIL;

	if ( grpwm_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( grpwm_register_io(&grpwm_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		grpwm_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */
	if ( grpwm_device_init(priv) ) {
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
		sprintf(priv->devName, "/dev/grpwm%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgrpwm%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, grpwm_driver_io_major,
			dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

static rtems_device_driver grpwm_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg);
static rtems_device_driver grpwm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grpwm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grpwm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grpwm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grpwm_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define GRPWM_DRIVER_TABLE_ENTRY { grpwm_initialize, grpwm_open, grpwm_close, grpwm_read, grpwm_write, grpwm_ioctl }

static rtems_driver_address_table grpwm_driver = GRPWM_DRIVER_TABLE_ENTRY;

int grpwm_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &grpwm_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("GRPWM driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			DBG("GRPWM rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			DBG("GRPWM rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			DBG("GRPWM rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			DBG("GRPWM rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

static void grpwm_scaler_set(
	struct grpwm_regs *regs,
	int scaler,
	unsigned int value)
{
	/* Select scaler */
	regs->ctrl = (regs->ctrl & ~GRPWM_CTRL_SCSEL) | (scaler << GRPWM_CTRL_SCSEL_BIT);
	/* Write scaler */
	regs->scaler = value;
}

/* Write Wave form RAM */
static void grpwm_write_wram(
	struct grpwm_regs *regs,
	unsigned int *data,
	int length)
{
	unsigned int *end;
	volatile unsigned int *pos;

	pos = &regs->wram[0];

	/* Write RAM */
	if ( data ) {
		end = data + length;
		while ( data < end ) {
			*pos++ = *data++;
		}
	} else {
		while( length > 0 ) {
			*pos++ = 0;
			length -= 4;
		}
	}
}

static void grpwm_hw_reset(struct grpwm_priv *priv)
{
	int i;
	struct grpwm_chan_priv *pwm;
	struct grpwm_regs *regs = priv->regs;

	/* Disable Core */
	regs->ctrl = 0;

	/* Clear all registers */
	regs->ipend = 0xffffffff;
	regs->wctrl = 0;

	/* Init all PWM channels */
	for (i=0; i<priv->channel_cnt; i++) {
		pwm = priv->channels[i];
		pwm->pwmregs->ctrl = 0;
		pwm->pwmregs->period = 0;
		pwm->pwmregs->comp = 0;
		pwm->pwmregs->dbcomp = 0;
		pwm->pwmregs->ctrl = 0; /* Twice because METH and POL requires EN=0 */
	}

	/* Clear RAM */
	if ( priv->wave ) {
		grpwm_write_wram(regs, NULL, priv->wlength);
	}

	/* Set max scaler */
	for (i=0; i<priv->nscalers; i++) {
		grpwm_scaler_set(regs, i, 0xffffffff);
	}
}

/* Update one Channel but leaves the "Hold update" bit set 
 *
 * A bit mask of updated bits are returned.
 */
static unsigned int grpwm_update_prepare_channel(
	struct grpwm_priv *priv,
	int channel,
	struct grpwm_ioctl_update_chan *up
	)
{
	struct grpwm_chan_priv *pwm;
	struct grpwm_pwm_regs *pwmregs;
	unsigned int ctrl;
	unsigned int ret;

	pwm = priv->channels[channel];
	pwmregs = pwm->pwmregs;

	/* Read channel control register */
	ctrl = pwmregs->ctrl;
	ret = 0;

	if ( up->options & GRPWM_UPDATE_OPTION_DISABLE ) {
		ctrl &= ~GRPWM_PCTRL_EN;
		pwmregs->ctrl = ctrl;
		ret |= GRPWM_PCTRL_EN;
	}

	/* Hold the updates */
	if ( up->options & (GRPWM_UPDATE_OPTION_PERIOD|
		GRPWM_UPDATE_OPTION_COMP|GRPWM_UPDATE_OPTION_DBCOMP) )  {

		if ( up->options & (GRPWM_UPDATE_OPTION_PERIOD) )  {
			DBG("GRPWM: UPDATING 0x%x: 0x%x\n", &pwmregs->period, up->period);
			pwmregs->period = up->period;
		}
		if ( up->options & (GRPWM_UPDATE_OPTION_COMP) )  {
			DBG("GRPWM: UPDATING 0x%x: 0x%x\n", &pwmregs->comp, up->compare);
			pwmregs->comp = up->compare;
		}
		if ( up->options & (GRPWM_UPDATE_OPTION_DBCOMP) )  {
			DBG("GRPWM: UPDATING 0x%x: 0x%x\n", &pwmregs->dbcomp, up->dbcomp);
			pwmregs->dbcomp = up->dbcomp;
		}
	}

	if ( up->options & GRPWM_UPDATE_OPTION_ENABLE ) {
		ret |= GRPWM_PCTRL_EN;
		pwmregs->ctrl = ctrl | GRPWM_PCTRL_EN;
	}
	return ret;
}

static void grpwm_update_active(struct grpwm_priv *priv, int enable)
{
	unsigned int ctrl;
	int i;

	ctrl = priv->regs->ctrl;

	/* Make all "Update Hold" bits be cleared */
	ctrl &= ~GRPWM_CTRL_NOUP;

	/* A change in any of the Channel enable/disable bits? */
	if ( enable ) {
		ctrl &= ~GRPWM_CTRL_EN;
		for(i=0; i<priv->channel_cnt; i++) {
			ctrl |= priv->regs->pwms[i].ctrl & GRPWM_CTRL_EN;
		}
	}
	priv->regs->ctrl = ctrl;
}

/* Configure the hardware of a channel according to this */
static rtems_status_code grpwm_config_channel(
	struct grpwm_priv *priv,
	int channel,
	struct grpwm_ioctl_config *cfg
	)
{
	struct grpwm_chan_priv *pwm;
	unsigned int pctrl, wctrl=0;

	pwm = priv->channels[channel];
	if ( pwm->pwmregs->ctrl & GRPWM_PCTRL_EN_BIT ) {
		return RTEMS_RESOURCE_IN_USE;
	}
	if ( cfg->options & ~GRPWM_CONFIG_OPTION_MASK ) {
		return RTEMS_INVALID_NAME;
	}
	if ( (cfg->options & GRPWM_CONFIG_OPTION_DUAL) && 
	     ((priv->regs->cap1 & GRPWM_CAP_DCM) == 0) ) {
		return RTEMS_INVALID_NAME;
	}
	/* IRQ set up */
	pwm->isr_arg = cfg->isr_arg;
	pwm->isr = cfg->isr;

	pctrl = cfg->options |
		(cfg->dbscaler << GRPWM_PCTRL_DBSC_BIT) |
		(cfg->irqscaler << GRPWM_PCTRL_ISC_BIT) |
		(cfg->scaler_index << GRPWM_PCTRL_SCSEL_BIT);

	/* Set Wave form gerneration if available */
	if ( !priv->wave || (priv->channel_cnt != (channel+1)) ) {
		/* Wave Form not available for this channel (or core) */
		if ( cfg->wave_activate || cfg->wave_data || cfg->wave_data_length ) {
			return RTEMS_INVALID_NAME;
		}
	} else if ( cfg->wave_activate ) {
		/* Enable Wave form generation */
		DBG("GRPWM: ENABLING WAVE FORM GENERATION 0x%x\n", cfg->wave_data_length);

		if ( cfg->wave_data ) {
			grpwm_write_wram(priv->regs, cfg->wave_data, cfg->wave_data_length);
		}

		/* Write length register, and let user control Wave-Sync functionality */
		wctrl = (((cfg->wave_data_length-1) << GRPWM_WCTRL_STOP_BIT) & GRPWM_WCTRL_STOP);
		wctrl |= cfg->wave_synccfg & (GRPWM_WCTRL_WSYNCCFG|GRPWM_WCTRL_WSEN);
		wctrl |= (cfg->wave_sync << 16) & 0x1fff0000;
		priv->regs->wctrl = wctrl;

		/* Enable Wave form */
		pctrl |= GRPWM_PCTRL_WEN;
	}

	DBG("GRPWM: CONFIG: 0x%x, WAVE CONFIG: 0x%x\n", pctrl, wctrl);

	pwm->pwmregs->ctrl = pctrl;

	return RTEMS_SUCCESSFUL;
}

static void grpwm_isr(void *arg)
{
	unsigned int ipend;
	struct grpwm_chan_priv *pwm = arg;
	struct grpwm_priv *priv = pwm->common;
	int i;

	/* Get current pending interrupts */
	ipend = priv->regs->ipend;

	for (i=0; i<priv->channel_cnt; i++) {
		if ( ipend & (1<<i) ) {
			pwm = priv->channels[i];
			if ( pwm->isr ) {
				pwm->isr(i, pwm->isr_arg);
			}
		}
	}
	priv->regs->ipend = ipend;
}

static rtems_device_driver grpwm_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg)
{
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grpwm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grpwm_priv *priv;
	rtems_device_driver ret;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&grpwm_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}
	priv = (struct grpwm_priv *)dev->priv;

	/* Wait until we get semaphore */
	if ( rtems_semaphore_obtain(priv->dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT) !=
	     RTEMS_SUCCESSFUL ){
		return RTEMS_INTERNAL_ERROR;
	}

	/* is device busy/taken? */
	if  ( priv->open ) {
		ret=RTEMS_RESOURCE_IN_USE;
		goto out;
	}

	/* Mark device taken */
	priv->open = 1;

	ret = RTEMS_SUCCESSFUL;
out:
	rtems_semaphore_release(priv->dev_sem);
	return ret;
}

static rtems_device_driver grpwm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grpwm_priv *priv;
	struct drvmgr_dev *dev;

	if ( drvmgr_get_dev(&grpwm_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	priv = (struct grpwm_priv *)dev->priv;

	/* Reset Hardware */
	grpwm_hw_reset(priv);

	/* Mark Device closed */
	priv->open = 0;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grpwm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	return RTEMS_UNSATISFIED;
}

static rtems_device_driver grpwm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	return RTEMS_UNSATISFIED;
}

static rtems_device_driver grpwm_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{

	struct grpwm_priv *priv;
	struct drvmgr_dev *dev;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *)arg;

	if ( drvmgr_get_dev(&grpwm_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NAME;
	}
	priv = (struct grpwm_priv *)dev->priv;

	if (!ioarg)
		return RTEMS_INVALID_NAME;

	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
		default: /* Not a valid command */
			return RTEMS_NOT_DEFINED;

		case GRPWM_IOCTL_GET_CAP:
		{
			struct grpwm_ioctl_cap *cap = (void *)ioarg->buffer;
			if ( cap == NULL )
				return RTEMS_INVALID_NAME;

			/* Copy Capability registers to user */
			cap->channel_cnt = priv->channel_cnt;
			cap->pwm = priv->regs->cap1;
			cap->wave = priv->regs->cap2;
			break;
		}
		case GRPWM_IOCTL_SET_CONFIG:
		{
			struct grpwm_ioctl_config *cfg = (void *)ioarg->buffer;
			if ( cfg == NULL )
				return RTEMS_INVALID_NAME;
			if ( cfg->channel >= priv->channel_cnt )
				return RTEMS_INVALID_NAME;

			return grpwm_config_channel(priv, cfg->channel, cfg);
		}
		case GRPWM_IOCTL_SET_SCALER:
		{
			unsigned int invalid_mask;
			int i;
			struct grpwm_ioctl_scaler *sc = ioarg->buffer;

			if ( sc == NULL )
				return RTEMS_INVALID_NAME;

			/* Test if caller reqest to set a scaler not existing */
			invalid_mask = ~((1 << priv->nscalers) - 1);
			if ( invalid_mask & sc->index_mask ) {
				return RTEMS_INVALID_NAME;
			}

			/* Set scalers requested */
			for (i=0; i<priv->nscalers; i++) {
				if ( sc->index_mask & (1<<i) ) {
					/* Update Scaler 'i' */
					grpwm_scaler_set(priv->regs, i, sc->values[i]);
				}
			}
			break;
		}
		case GRPWM_IOCTL_UPDATE:
		{
			struct grpwm_ioctl_update *up = ioarg->buffer;
			unsigned int invalid_mask, pctrl = 0;
			int i;

			if ( up == NULL )
				return RTEMS_INVALID_NAME;

			/* Test if caller reqest to set a scaler not existing */
			invalid_mask = ~((1 << priv->channel_cnt) - 1);
			if ( invalid_mask & up->chanmask ) {
				return RTEMS_INVALID_NAME;
			}

			/* In order for the changes to take effect at the same time, the "Hold update"
			 * bits is set for all PWM channels that will be updated. The hold update bits
			 * will be cleared at the same time for all channels.
			 */
			priv->regs->ctrl = (priv->regs->ctrl & ~GRPWM_CTRL_NOUP) | 
				(up->chanmask << GRPWM_CTRL_NOUP_BIT);

			for (i=0; i<priv->channel_cnt; i++) {
				if ( up->chanmask & (1<<i) ) {
					/* Prepare update channel 'i' */
					pctrl |= grpwm_update_prepare_channel(priv, i, &up->channels[i]);
				}
			}

			/* 1. Update all channels requested, 
			 * 2. Enable the core if at least one channel is enabled
			 * 3. Disable the core if all channels are disabled
			 */
			grpwm_update_active(priv, (pctrl & GRPWM_PCTRL_EN));

			break;
		}
		case GRPWM_IOCTL_IRQ:
		{
			unsigned int data = (unsigned int)ioarg->buffer;
			int channel = (data >> 8) & 0x7;
			struct grpwm_chan_priv *pwm;
			unsigned int pctrl;

			pwm = priv->channels[channel];

			if ( data & GRPWM_IRQ_CLEAR ) {
				priv->regs->ipend |= (1<<channel);
				drvmgr_interrupt_clear(priv->dev, pwm->irqindex);
			}
			if ( (data & 0x3) && !pwm->isr ) {
				/* Enable IRQ but no ISR */
				return RTEMS_INVALID_NAME;
			}
			pctrl = pwm->pwmregs->ctrl & ~(GRPWM_PCTRL_IEN|GRPWM_PCTRL_IT);
			pctrl |= ((data & 0x3) << GRPWM_PCTRL_IEN_BIT);
			pwm->pwmregs->ctrl = pctrl;
			break;
		}
	}

	return RTEMS_SUCCESSFUL;
}

#define MAX_CHANNEL 8
char grpwm_irqindex_lookup[8][MAX_CHANNEL] = 
{
/* Channel       1  2  3  4  5  6  7  8 */
/* npwm 1 */	{0, 0, 0, 0, 0, 0, 0, 0},
/* npwm 2 */	{0, 1, 0, 0, 0, 0, 0, 0},
/* npwm 3 */	{0, 0, 0, 0, 0, 0, 0, 0},
/* npwm 4 */	{0, 0, 0, 1, 0, 0, 0, 0},
/* npwm 5 */	{0, 0, 0, 1, 2, 0, 0, 0},
/* npwm 6 */	{0, 0, 0, 1, 1, 1, 0, 0},
/* npwm 7 */	{0, 0, 0, 1, 1, 1, 2, 0},
/* npwm 8 */	{0, 0, 0, 1, 1, 1, 2, 3}
};

int grpwm_device_init(struct grpwm_priv *priv)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	int mask, i, sepirq;
	unsigned int wabits;
	struct grpwm_chan_priv *pwm;
	struct grpwm_regs *regs;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	priv->irq = pnpinfo->irq;
	regs = priv->regs = (struct grpwm_regs *)pnpinfo->apb_slv->start;

	DBG("GRPWM: 0x%08x irq %d\n", (unsigned int)regs, priv->irq);

	/* Disable Core */
	regs->ctrl = 0;

	/* Clear all registers */
	regs->ipend = 0xffffffff;
	regs->wctrl = 0;

	/* Find the number of PWM channels */
	priv->channel_cnt = 1 + ((regs->cap1 & GRPWM_CAP_NPWM) >> GRPWM_CAP_NPWM_BIT);
	pwm = grlib_calloc(priv->channel_cnt, sizeof(*pwm));
	if ( !pwm )
		return -1;

	/* Init all PWM channels */
	sepirq = ((regs->cap1 & GRPWM_CAP_SEP) >> GRPWM_CAP_SEP_BIT);
	for (i=0; i<priv->channel_cnt; i++, pwm++) {
		priv->channels[i] = pwm;
		pwm->common = priv;
		pwm->pwmregs = &regs->pwms[i];
		if ( sepirq == 0 ) {
			pwm->irqindex = 0;
		} else if ( sepirq == 1 ) {
			pwm->irqindex = i;
		} else {
			pwm->irqindex = grpwm_irqindex_lookup[priv->channel_cnt][i];
		}
	}

	/* Detect if Wave Form capability is availble for last PWM channel */
	if ( regs->cap2 & GRPWM_CAP2_WPWM ) {
		priv->wave = 1;

		/* Clear RAM */
		wabits = (regs->cap2 & GRPWM_CAP2_WABITS) >> GRPWM_CAP2_WABITS_BIT;
		priv->wlength = 1 << wabits;
	}
	priv->nscalers = 1 + ((regs->cap1 & GRPWM_CAP_NSC) >> GRPWM_CAP_NSC_BIT);

	grpwm_hw_reset(priv);

	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'P', 'W', 'M'),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&priv->dev_sem) != RTEMS_SUCCESSFUL ) {
		return -1;
	}

	/* Register interrupt handler for all PWM channels */
	mask = 0;
	for (i=0; i<priv->channel_cnt; i++) {
		pwm = priv->channels[i];
		if ( (mask & (1 << pwm->irqindex)) == 0 ) {
			/* Not registered interrupt handler for this IRQ index before,
			 * we do it now.
			 */
			mask |= (1 << pwm->irqindex);
			drvmgr_interrupt_register(
				priv->dev,
				pwm->irqindex,
				"grpwm",
				grpwm_isr,
				pwm);
		}
	}

	return 0;
}
